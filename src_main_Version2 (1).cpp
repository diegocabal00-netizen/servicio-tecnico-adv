#include <Arduino.h>
#include <M5Cardputer.h>
#include <Preferences.h>
#include <WebServer.h>
#include <WiFi.h>

namespace {
constexpr char kApSsid[] = "TechDiag-CardputerAdv";
constexpr char kApPassword[] = "12345678";

WebServer server(80);
Preferences preferences;

String savedSsid;
String savedPassword;

const char kIndexHtml[] PROGMEM = R"HTML(<!doctype html>
<html lang="es">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>TechDiagnostic Pro</title>
  <style>
    :root {
      color-scheme: dark;
      font-family: system-ui, sans-serif;
      background: #10151d;
      color: #eef3f8;
    }
    body {
      max-width: 760px;
      margin: 2rem auto;
      padding: 0 1rem;
    }
    header, .card {
      background: #1b2532;
      border: 1px solid #344456;
      border-radius: 14px;
      padding: 1.2rem;
      margin: 1rem 0;
    }
    h1 { margin: .2rem 0; color: #6ee7b7; }
    h2 { font-size: 1.15rem; }
    p, .muted { color: #bdc9d6; }
    .grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
      gap: .8rem;
    }
    .metric {
      padding: .8rem;
      background: #111923;
      border-radius: 9px;
    }
    label {
      display: block;
      margin: .7rem 0 .25rem;
    }
    input, button {
      box-sizing: border-box;
      width: 100%;
      padding: .8rem;
      border-radius: 8px;
      border: 1px solid #52677e;
      background: #101923;
      color: white;
      font: inherit;
    }
    button {
      margin-top: 1rem;
      background: #087f5b;
      border: 0;
      font-weight: 700;
      cursor: pointer;
    }
    .note {
      font-size: .9rem;
      color: #ffd782;
    }
    #message { min-height: 1.4em; }
  </style>
</head>
<body>
  <header>
    <h1>TechDiagnostic Pro</h1>
    <p>Panel local para M5Stack Cardputer Adv.</p>
    <p class="note">La red de configuración usa una clave predeterminada. Cámbiala en el código antes de usar el equipo en un entorno no controlado.</p>
  </header>

  <section class="card">
    <h2>Estado del dispositivo</h2>
    <div class="grid">
      <div class="metric">Wi-Fi taller<br><strong id="wifi">Consultando…</strong></div>
      <div class="metric">IP local<br><strong id="ip">—</strong></div>
      <div class="metric">Tiempo encendido<br><strong id="uptime">—</strong></div>
      <div class="metric">Memoria libre<br><strong id="heap">—</strong></div>
    </div>
    <p class="muted">Esta página ofrece diagnóstico básico de conectividad y estado; no ejecuta pruebas eléctricas sobre el teléfono conectado.</p>
  </section>

  <section class="card">
    <h2>Conectar a la Wi-Fi del taller</h2>
    <form id="wifiForm">
      <label for="ssid">Nombre de red (SSID)</label>
      <input id="ssid" name="ssid" maxlength="32" autocomplete="off" required>
      <label for="password">Contraseña Wi-Fi</label>
      <input id="password" name="password" type="password" maxlength="63" autocomplete="new-password">
      <button type="submit">Guardar y conectar</button>
    </form>
    <p id="message" role="status"></p>
    <p class="muted">La red de configuración seguirá activa mientras se conecta a la red guardada. Las credenciales se guardan en la memoria del dispositivo.</p>
  </section>

  <script>
    async function refresh() {
      try {
        const r = await fetch('/api/status');
        const s = await r.json();
        document.querySelector('#wifi').textContent = s.connected ? 'Conectado' : 'Sin conexión';
        document.querySelector('#ip').textContent = s.connected ? s.local_ip : '192.168.4.1 (configuración)';
        document.querySelector('#uptime').textContent = s.uptime;
        document.querySelector('#heap').textContent = s.free_heap + ' bytes';
      } catch (e) {
        document.querySelector('#wifi').textContent = 'Sin respuesta';
      }
    }

    document.querySelector('#wifiForm').addEventListener('submit', async e => {
      e.preventDefault();
      const m = document.querySelector('#message');
      m.textContent = 'Guardando y conectando…';

      try {
        const r = await fetch('/api/wifi', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: new URLSearchParams(new FormData(e.target))
        });

        const d = await r.json();
        m.textContent = d.message;
        if (r.ok) {
          document.querySelector('#password').value = '';
          setTimeout(refresh, 1000);
        }
      } catch (err) {
        m.textContent = 'No se pudo contactar con el dispositivo';
      }
    });

    refresh();
    setInterval(refresh, 5000);
  </script>
</body>
</html>
)HTML";

void sendJson(int statusCode, const String &body) {
  server.sendHeader("Cache-Control", "no-store");
  server.send(statusCode, "application/json; charset=utf-8", body);
}

void drawStartupScreen() {
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.drawString("TechDiagnostic", 8, 8);

  M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.drawString("Cardputer Adv | Diagnostico", 8, 36);
  M5Cardputer.Display.drawString("AP: TechDiag-CardputerAdv", 8, 58);
  M5Cardputer.Display.drawString("Clave inicial: 12345678", 8, 74);
  M5Cardputer.Display.drawString("Panel: http://192.168.4.1", 8, 90);
  M5Cardputer.Display.drawString("Conecta primero a la red AP", 8, 114);
}

void handleStatus() {
  const bool connected = WiFi.status() == WL_CONNECTED;
  const String ip = connected ? WiFi.localIP().toString() : String("192.168.4.1");
  const uint32_t uptimeSeconds = millis() / 1000;
  const uint32_t hours = uptimeSeconds / 3600;
  const uint32_t minutes = (uptimeSeconds % 3600) / 60;
  const uint32_t seconds = uptimeSeconds % 60;

  char uptime[24];
  snprintf(
    uptime,
    sizeof(uptime),
    "%lu h %02lu min %02lu s",
    static_cast<unsigned long>(hours),
    static_cast<unsigned long>(minutes),
    static_cast<unsigned long>(seconds)
  );

  String body = "{\"connected\":";
  body += connected ? "true" : "false";
  body += ",\"local_ip\":\"" + ip + "\",\"uptime\":\"" + uptime;
  body += "\",\"free_heap\":" + String(ESP.getFreeHeap()) + "}";
  sendJson(200, body);
}

void handleWifiConfig() {
  if (!server.hasArg("ssid") || server.arg("ssid").isEmpty()) {
    sendJson(400, "{\"message\":\"Escribe el nombre de la red.\"}");
    return;
  }

  const String ssid = server.arg("ssid");
  const String password = server.arg("password");

  if (ssid.length() > 32 || password.length() > 63) {
    sendJson(400, "{\"message\":\"SSID o contraseña demasiado largos.\"}");
    return;
  }

  preferences.putString("ssid", ssid);
  preferences.putString("password", password);

  savedSsid = ssid;
  savedPassword = password;
  WiFi.begin(savedSsid.c_str(), savedPassword.c_str());

  sendJson(200, "{\"message\":\"Credenciales guardadas; intentando conectar.\"}");
}

void connectSavedNetwork() {
  savedSsid = preferences.getString("ssid", "");
  savedPassword = preferences.getString("password", "");

  if (!savedSsid.isEmpty()) {
    Serial.printf("Intentando conectar a la red guardada: %s\n", savedSsid.c_str());
    WiFi.begin(savedSsid.c_str(), savedPassword.c_str());
  }
}
}  // namespace

void setup() {
  Serial.begin(115200);

  auto config = M5.config();
  M5Cardputer.begin(config, true);
  M5Cardputer.Display.setRotation(1);
  drawStartupScreen();

  preferences.begin("techdiag", false);

  WiFi.mode(WIFI_AP_STA);
  WiFi.setSleep(false);

  const IPAddress apIp(192, 168, 4, 1);
  WiFi.softAPConfig(apIp, apIp, IPAddress(255, 255, 255, 0));

  if (!WiFi.softAP(kApSsid, kApPassword)) {
    Serial.println("Error al iniciar el punto de acceso");
  }

  connectSavedNetwork();

  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html; charset=utf-8", kIndexHtml);
  });

  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/wifi", HTTP_POST, handleWifiConfig);

  server.onNotFound([]() {
    sendJson(404, "{\"message\":\"Ruta no encontrada.\"}");
  });

  server.begin();
  Serial.printf("Punto de acceso listo: %s en http://%s/\n", kApSsid, apIp.toString().c_str());
}

void loop() {
  server.handleClient();
  delay(2);
}