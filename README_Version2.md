# TechDiagnostic Pro ONLINE - M5Stack Cardputer Adv

Esta versión está diseñada para compilarse y usarse 100% online, sin necesidad de instalar programas pesados en tu PC.

## 1. Compilar el `.bin` Online en la Nube (GitHub Actions)

1. Sube el contenido de esta carpeta a un repositorio gratuito en GitHub.
2. Ve a la pestaña Actions y ejecuta Build Cardputer Adv Firmware Online.
3. Descarga el archivo firmware.bin generado automáticamente.

## 2. Flashear Online desde el Navegador (Web Flasher)

1. Conecta tu Cardputer Adv por USB-C a Chrome o Edge.
2. Abre ESP Web Tool / ESPTool JS o usa M5Launcher desde la tarjeta SD.
3. Carga el firmware.bin en la dirección 0x10000.
   - Nota: el HTML/CSS/JS está embebido dentro del propio bin, por lo que funciona en un solo archivo.

## 3. Uso Online en el Taller

1. Al encender, conéctate a la red TechDiag-CardputerAdv (clave 12345678).
2. Abre http://192.168.4.1 o http://techdiag.local.
3. En la pestaña Conectar Cardputer Adv a Wi-Fi, guarda la red Wi-Fi del taller.
4. El dispositivo intentará reconectarse automáticamente tras reiniciarse.

## Seguridad

La clave del punto de acceso es una clave inicial conocida. Debes cambiarla en src/main.cpp antes de usarlo fuera de entorno de prueba.

## Alcance

Este firmware sirve para:
- mostrar información básica del estado del Cardputer
- servir una página web local para conexión Wi-Fi
- guardar credenciales Wi-Fi en la memoria NVS
- diagnosticar conectividad básica local

No realiza mediciones eléctricas ni pruebas sobre teléfonos ni equipos externos; para eso se requiere hardware y software adicionales.