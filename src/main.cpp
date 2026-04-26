#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include "secrets.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);

WiFiClientSecure client;
HTTPClient http;
String resposta;
String direcaoVento = "";

void conectarWiFi();
void garantirWiFiConectado();
void seguranca();
void preparacaoHttp();
void consultarAPI();
void LocalizacaoCondicaoGeral();
void TemperaturaUmidade();
void direcaoVentos();
void VentosOutrosDados();

void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
}

void loop()
{

}

void conectarWiFi()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("==================");
  lcd.setCursor(0, 1);
  lcd.print("Conectando WiFi...");
  lcd.setCursor(0, 2);
  lcd.print("==================");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_SENHA);

  int tentativas = 0;
  const int maxTentativas = 30;
  
  while (WiFi.status() != WL_CONNECTED && tentativas < maxTentativas)
  {
    delay(500);
    lcd.setCursor(tentativas % 20, 1);
    lcd.print(".");
    tentativas++;
  }

  lcd.clear();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi Conectado");
    Serial.print("IP: ");
    Serial.print(WiFi.localIP());

    lcd.setCursor(0, 0);
    lcd.println("================");
    lcd.setCursor(0, 1);
    lcd.print("WiFi: Conectado!");
    lcd.setCursor(0, 2);
    lcd.print(WiFi.localIP().toString());
    lcd.setCursor(0, 3);
    lcd.println("================");
  }
  else
  {
    Serial.println("Falha na conexao");
    lcd.setCursor(0, 0);
    lcd.print("====================");
    lcd.setCursor(0, 1);
    lcd.print("Erro no WiFi!");
    lcd.setCursor(0, 2);
    lcd.print("Verefique rede/senha");
    lcd.setCursor(0, 3);
    lcd.print("====================");
  }
}

void garantirWiFiConectado()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    lcd.setCursor(0, 0);
    lcd.print("====================");
    lcd.setCursor(0, 1);
    lcd.println("WiFi desconectado.");
    lcd.setCursor(0, 2);
    lcd.print("Tentando reconectar...");
    lcd.setCursor(0, 3);
    lcd.print("====================");
    conectarWiFi();
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    lcd.setCursor(0, 0);
    lcd.print("============================");
    lcd.setCursor(0, 1);
    lcd.print("Não foi possivel reconectar.");
    lcd.setCursor(0, 3);
    lcd.print("============================");
  }
}

void seguranca()
{
  client.setInsecure();
}

void preparacaoHttp()
{
  if (!http.begin(client, URL_API))
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("===================="); // 20 caracteres exatos
    lcd.setCursor(0, 1);
    lcd.print("Erro: Conexao HTTP");
    lcd.setCursor(0, 2);
    lcd.print("Verifique a URL");
    lcd.setCursor(0, 3);
    lcd.print("====================");
    return;
  }
  http.setTimeout(10000);
  consultarAPI();
}

void consultarAPI()
{
  int httpCode = http.GET();

  if (httpCode > 0)
  {
    Serial.print("Código HTTP: ");
    Serial.println(httpCode);

    if (httpCode == HTTP_CODE_OK)
    {
      resposta = http.getString();
    }
    else
    {
      Serial.print("A API respondeu, mas com codigo de erro: ");
      Serial.print(httpCode);
    }
  }
  else
  {
    Serial.print("Erro de requisição HTTP: ");
    Serial.print(http.errorToString(httpCode));
  }
  http.end();
}

void LocalizacaoCondicaoGeral()
{
  JsonDocument doc;

  DeserializationError erro = deserializeJson(doc, resposta);

  if (!erro)
  {
    lcd.clear();
    String cidade = "";
    String estado = "";
    String horario = "";
    String clima = "";

    if (doc["location"]["name"].is<JsonVariant>())
    {
      cidade = doc["location"]["name"].as<String>();
    }
    if (doc["location"]["region"].is<JsonVariant>())
    {
      estado = doc["location"]["region"].as<String>();
    }
    if (doc["location"]["localtime"].is<JsonVariant>())
    {
      horario = doc["location"]["localtime"].as<String>();
      horario = horario.substring(11);
    }
    if (doc["current"]["condition"]["text"].is<JsonVariant>())
    {
      clima = doc["current"]["condition"]["text"].as<String>();
    }

    lcd.setCursor(0, 0);
    lcd.print(cidade);

    lcd.setCursor(0, 1);
    lcd.print(estado);

    lcd.setCursor(0, 2);
    lcd.print(horario);

    lcd.setCursor(0, 3);
    lcd.print(clima);
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro no JSON");
  }
  doc.clear();
}

void TemperaturaUmidade()
{
  JsonDocument doc;

  DeserializationError erro = deserializeJson(doc, resposta);

  if (!erro)
  {
    lcd.clear();
    float temperaturaAtual;
    float sensacaoTermica;
    int umidade;
    int nebulosidade;

    if (doc["current"]["temp_c"].is<JsonVariant>())
    {
      temperaturaAtual = doc["current"]["temp_c"];
    }
    if (doc["current"]["feelslike_c"].is<JsonVariant>())
    {
      sensacaoTermica = doc["current"]["feelslike_c"];
    }
    if (doc["current"]["humidity"].is<JsonVariant>())
    {
      umidade = doc["current"]["humidity"];
    }
    if (doc["current"]["cloud"].is<JsonVariant>())
    {
      nebulosidade = doc["current"]["cloud"];
    }

    lcd.setCursor(0, 0);
    lcd.printf("Temp: %.1fC", temperaturaAtual);

    lcd.setCursor(0, 1);
    lcd.printf("Sens: %.1fC", sensacaoTermica);

    lcd.setCursor(0, 2);
    lcd.printf("Umidade: %.1d%%", umidade);

    lcd.setCursor(0, 3);
    lcd.printf("Nuvens: %.1d%%", nebulosidade);
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Erro no JSON");
  }
  doc.clear();
}

void direcaoVentos()
{
  if (direcaoVento == "N")
    {
      direcaoVento = "Norte ";
    }
    else if (direcaoVento == "E")
    {
      direcaoVento = "Leste";
    }
    else if (direcaoVento == "S")
    {
      direcaoVento = "Sul";
    }
    else if (direcaoVento == "W")
    {
      direcaoVento = "Oeste";
    }
    else if (direcaoVento == "NE")
    {
      direcaoVento = "Nordeste";
    }
    else if (direcaoVento == "SE")
    {
      direcaoVento = "Sudeste";
    }
    else if (direcaoVento == "SO")
    {
      direcaoVento = "Sudoeste";
    }
    else if (direcaoVento == "NO")
    {
      direcaoVento = "Noroeste";
    }
    else if (direcaoVento == "NNE")
    {
      direcaoVento = "Nor-nordeste";
    }
    else if (direcaoVento == "NNO")
    {
      direcaoVento = "Nor-noroeste";
    }
    else if (direcaoVento == "ENE")
    {
      direcaoVento = "Les-nordeste";
    }
    else if (direcaoVento == "ESE")
    {
      direcaoVento = "Les-sudeste";
    }
    else if (direcaoVento == "SSE")
    {
      direcaoVento = "Sul-sudeste";
    }
    else if (direcaoVento == "SSO")
    {
      direcaoVento = "Sul-sudoeste";
    }
    else if (direcaoVento == "OSO")
    {
      direcaoVento = "Oes-sudoeste";
    }
    else
    {
      direcaoVento = "Oes-noroeste";
    }
}

void VentosOutrosDados()
{
  JsonDocument doc;

  DeserializationError erro = deserializeJson(doc, resposta);

  if (!erro)
  {
    lcd.clear();
    float velocidadeVento;
    String direcaoVento = "";
    int pressao;
    float indiceUV;

    if (doc["current"]["wind_kph"].is<JsonVariant>())
    {
      velocidadeVento = doc["current"]["wind_kph"];
    }
    if (doc["current"]["wind_dir"].is<String>())
    {
      direcaoVento = doc["current"]["wind_dir"].as<String>();
    }
    if (doc["current"]["pressure_mb"].is<JsonVariant>())
    {
      pressao = doc["current"]["pressure_mb"];
    }
    if (doc["current"]["uv"].is<JsonVariant>())
    {
      indiceUV = doc["current"]["uv"];
    }

    direcaoVentos();
    
    lcd.setCursor(0, 0);
    lcd.printf("Vento: %.1fkm/h", velocidadeVento);

    lcd.setCursor(0, 1);
    lcd.printf("Dir: %s", direcaoVento.c_str());

    lcd.setCursor(0, 2);
    lcd.printf("Pressao: %.1dmb", pressao);

    lcd.setCursor(0, 3);
    lcd.printf("UV: %.1f", indiceUV);
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Erro no JSON");
  }
  doc.clear();
}
