# TechDiagnostic Pro ONLINE - M5Stack Cardputer Adv

Esta versión está diseñada para **compilarse y usarse 100% online**, sin necesidad de instalar programas pesados en tu PC:

## 1. Compilar el `.bin` Online en la Nube (GitHub Actions)
1. Sube el contenido de esta carpeta a un repositorio gratuito en **GitHub**.
2. Ve a la pestaña **Actions** en GitHub y ejecuta **Build Cardputer Adv Firmware Online**.
3. Descarga el archivo `firmware.bin` generado automáticamente.

## 2. Flashear Online desde el Navegador (Web Flasher)
1. Conecta tu **Cardputer Adv** por USB-C a Chrome o Edge.
2. Abre [ESP Web Tool / ESPTool JS](https://espressif.github.io/esptool-js/) o usa **M5Launcher** desde la tarjeta SD del Cardputer Adv.
3. Carga el `firmware.bin` en la dirección `0x10000` (o en la carpeta `/apps` de la SD si usas M5Launcher).
   - *Nota:* El código HTML/CSS/JS viene **embebido dentro del propio `.bin`**, por lo que funciona al instante con un solo archivo.

## 3. Uso Online en el Taller
1. Al encender, conéctate a la red `TechDiag-CardputerAdv` (clave `12345678`) y abre `http://192.168.4.1` o `http://techdiag.local`.
2. En la pestaña **Conectar Cardputer Adv a Wi-Fi**, guarda la red Wi-Fi de tu taller para controlarlo desde cualquier dispositivo conectado a tu red local.