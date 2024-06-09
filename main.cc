#include <menu/menu.hpp>
#include <array>

#include "bluetooth.hpp"

#define MAC "FC:3B:15:35:54:F7"

struct Time {
   uint8_t year{};
   uint8_t mon{};
   uint8_t mday{};
   uint8_t hour{};
   uint8_t min{};
   uint8_t sec{};
   uint8_t is12Hour{};
};

std::array<uint8_t, 20> create_message(const Time& time);

static BLEPP::BLEGATTStateMachine s_gatt;

void init();
void menu_synchronize();
tm* get_current_time();

int main() {
   if(!is_bluetooth_open()) {
      ECHO_ERROR("Error al intentar leer el driver del Bluetooth. Verifique si está instalado o encendido.\n");
      return 1;
   }

   init();

   menu::Menu main_menu(
      menu::make_item("Sincronizar.", menu_synchronize)
   );
   main_menu.loop("SMARTWATCH ver 1.2 by phobos");

   return 0;
}

void init() {
   BLEPP::log_level = (BLEPP::LogLevels)-1;

   s_gatt.cb_disconnected = [](auto d) {
      if(d.reason != BLEPP::BLEGATTStateMachine::Disconnect::ConnectionClosed) {
         ECHO_ERROR("BLEPP: Disconnected for reason '%s'\n", BLEPP::BLEGATTStateMachine::get_disconnect_string(d));
         exit(1);
      }
   };
}

void menu_synchronize() {
   menu::titulo("SINCRONIZAR");

   ECHO("Intentando conectarse al dispositivo '{}'...\n", MAC);
   try {
      s_gatt.connect_blocking(MAC);
   } catch(const std::runtime_error& e) {
      ECHO_ERROR("Error al intentar conectarse con el dispositivo: {}\n", e.what());
      return;
   }

   ECHO_INFO("Dispositivo conectado!!!\n");

   bool is12format = menu::force_input<bool>("12-format? [0/1]: ");
   ECHO_HIGHLIGHT("{} establecido!!!\n", is12format ? "12-format" : "24-format");

   auto tm_s = get_current_time();
   auto bytes = create_message({
      .year=(tm_s->tm_year+1900)%100,
      .mon=tm_s->tm_mon+1,
      .mday=tm_s->tm_mday,
      .hour=tm_s->tm_hour,
      .min=tm_s->tm_min,
      .sec=tm_s->tm_sec,
      .is12Hour=is12format
   });
   s_gatt.send_write_request(0x0026, bytes.data(), bytes.size());
   ECHO_HIGHLIGHT("Reloj configurado {}:{}:{} {}/{}/{} {}\n",
      bytes[16],
      bytes[17],
      bytes[18],
      bytes[15],
      bytes[14],
      bytes[13],
      bytes[19] ? "12-format" : "24-format");
}

std::array<uint8_t, 20> create_message(const Time& time) {
   return {
      0xba,
      0x20,
      0x00,
      0x0c,
      0x00,
      0x66,
      0x00,
      0x05,
      0x02,
      0x00,
      0x20,
      0x00,
      0x07,
      time.year,
      time.mon,
      time.mday,
      time.hour,
      time.min,
      time.sec,
      time.is12Hour,
   };
}

tm* get_current_time() {
   time_t t = time(nullptr);
   return localtime(&t);
}