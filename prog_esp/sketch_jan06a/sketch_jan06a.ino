#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <Adafruit_NeoPixel.h>

#define PIN 2
#define NB_LED 57

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NB_LED, PIN, NEO_GRB + NEO_KHZ800);



const char* ssid = "freebox_RXCGSQ";
const char* pass = "baptistewentzler";

IPAddress staticIP(192,168,1,99);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
//IPAddress dns(8,8,8,8);


//WiFiServer server(80);
ESP8266WebServer server(80);

// globals vars
int intensite = 128;
int lastIntensite = 0;
String couleur = "#FF0000";
String lastCouleur = "";
String effet = "Aucun";
String lastEffet = "";

// functions
void handleled();
void handleintensite();
void clickOn();
void clickOff();
void setColor(String); // color in hex format #RRGGBB
void convertCoul(int *, String); 



void setup() {

  Serial.begin(115200);
  delay(10);

  strip.begin();
  strip.show();

  Serial.println();
  Serial.print("Connexion au wifi ");
  Serial.print(ssid);
  WiFi.config(staticIP, subnet, gateway);
  WiFi.begin(ssid,pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.print("OK");
  Serial.println();


  // test de la memoire
  if (!SPIFFS.begin())
  {
    Serial.println("impossible de monter le stockage");
  }
  else
  {
    Serial.println("montage ok");
  }

  // fichiers utilises par le serveur web
  server.serveStatic("/",SPIFFS,"/index.html");
  server.serveStatic("/led_strip.png",SPIFFS,"/led_strip.png");
  server.serveStatic("/style.css",SPIFFS,"/style.css");
  server.serveStatic("/script.js",SPIFFS,"/script.js");

  // demarrage du serveur web
  server.on("/LED",HTTP_GET,handleled);
  server.on("/ledOn", HTTP_GET,clickOn);
  server.on("/ledOff", HTTP_GET,clickOff);
  server.on("/status", HTTP_GET,status);

  server.begin();
  Serial.println("Serveur web OK");
  Serial.println(WiFi.localIP());

}

void loop() {
 
  server.handleClient();
}




// function calls when "Envoyer" button is clicked
void handleled()
  {
    Serial.println("Reception de donnees:");


    // si reception couleur
    if(server.arg("couleur") != ""){

      Serial.print("couleur: ");
      Serial.println(server.arg("couleur"));
      
      couleur = server.arg("couleur");
      setColor(server.arg("couleur"));
      server.send(200, "text/plain", "on");
      return;     
    }
    // si reception intensite
    if(server.arg("intensite") != ""){
      Serial.print("Intensité: ");
      
      intensite = server.arg("intensite").toInt();
      Serial.println(intensite);
      strip.setBrightness(intensite);
      strip.show();
      if(effet == "Aucun"){
        setColor(couleur);
      }
      

      if(intensite == 0){
        return server.send(200, "text/plain", "off");
      }
      server.send(200, "text/plain", "on");
      return;
    }

    // si changement d effet
    if(server.arg("effet") != ""){
      Serial.print("Effet: ");
      Serial.println(server.arg("effet"));
      effet = server.arg("effet");

      if(effet == "France"){
        effetFrance(NB_LED);
        server.send(200, "text/plain", "on");
        return;
      }
      
      server.send(200, "text/plain", "on");
      return;
    }

    server.send(418);
  }

// function calls when "On" button is clicked
void clickOn()
  {
    Serial.println("Led ON");
    //server.sendHeader("Location","/");
    server.send(200, "text/plain", "on");
    // light on rgb strip with the saved color
    setColor(couleur);

  }

// function calls when "Off" button is clicked
void clickOff(){
  strip.clear();
  strip.show();
  Serial.println("Led Off");
  //server.sendHeader("Location","/");
  server.send(200, "text/plain", "off");

}
// status retourne json 
void status(){
  String resp = "{\"intensite\":\"";
  resp += String(intensite) + "\",\"couleur\":\"" + couleur + "\",\"effet\":\"" + effet + "\"}";
  Serial.println(intensite);
  server.send(200, "text/json", resp);
}
// drapeau francais
void effetFrance(int nb_led){
  int bleu = nb_led / 3;
  int blanc = bleu * 2;
  int rouge = nb_led;

  for(int i = 0; i < bleu; i++){
    strip.setPixelColor(i,0,0,255);
  }
  for(int i = bleu; i < blanc; i++){
    strip.setPixelColor(i,255,255,255);
  }
  for(int i = blanc; i < rouge; i++){
    strip.setPixelColor(i,255,0,0);
  }
  strip.show();

}


//------------------------------------------------------------------------------

// define colors coul = #RRGGBB in hexa
void setColor(String coul)
{
  int rgbint[3];

  //convert hexa color to int
  convertCoul(rgbint, coul);

  Serial.print("INT R: ");
  Serial.print(rgbint[0]);
  Serial.print(" G: ");
  Serial.print(rgbint[1]);
  Serial.print(" B: ");
  Serial.println(rgbint[2]);

  Serial.println("eclairage led");
  for(int i=0; i<NB_LED; i++)
  {
    strip.setPixelColor(i,rgbint[0],rgbint[1],rgbint[2]);

  }
  strip.show();

}

// convert color from #RRGGBB in hex to int 
void convertCoul(int *rgbint, String coul){
  Serial.println(coul);

  int val = 0;
  String  rgb[3];
  rgb[0] = coul[1];
  rgb[0] += coul[2]; //String R

  rgb[1] = coul[3];
  rgb[1] += coul[4]; // String G

  rgb[2] = coul[5] ;
  rgb[2] += coul[6]; // String B
  
  Serial.print("HEX R: ");
  Serial.print(rgb[0]);
  Serial.print(" G: ");
  Serial.print(rgb[1]);
  Serial.print(" B: ");
  Serial.println(rgb[2]);
  

  for (int j = 0; j<3;j++){
    val = 0;
    for (int i= 0; i<2;i++)
    {
     switch (rgb[j][i])
     {
      case '0':
      val += 0;
      break;
      
      case '1':
      if(i == 0){
        val = 1* pow(16,1);
      }
      else{
        val += 1*pow(16,0);
      }
      break;
      
      case '2':
      if(i==0){
        val = 2*pow(16,1);
      }
      else{
        val +=2*pow(16,0);
      }
      break;
      case '3':
      if(i==0){
        val = 3*pow(16,1);
      }
      else{
        val +=3*pow(16,0);
      }
      
      break;
      case '4':
      if(i==0){
        val = 4*pow(16,1);
      }
      else{
        val +=4*pow(16,0);
      }
      break;
      case '5':
      if(i==0){
        val = 5*pow(16,1);
      }
      else{
        val +=5*pow(16,0);
      }
      break;
      case '6':
      if(i==0){
        val = 6*pow(16,1);
      }
      else{
        val +=6*pow(16,0);
      }
      break;
      case '7':
      if(i==0){
        val = 7*pow(16,1);
      }
      else{
        val +=7*pow(16,0);
      }
      break;
      case '8':
      if(i==0){
        val = 8*pow(16,1);
      }
      else{
        val +=8*pow(16,0);
      }
      break;
      case '9':
      if(i==0){
        val = 9*pow(16,1);
      }
      else{
        val +=9*pow(16,0);
      }
      break;
      case 'A':
      if(i==0){
        val = 10*pow(16,1);
      }
      else{
        val +=10*pow(16,0);
      }
      break;
      case 'a':
      if(i==0){
        val = 10*pow(16,1);
      }
      else{
        val +=10*pow(16,0);
      }
      break;
      case 'B':
      if(i==0){
        val = 11*pow(16,1);
      }
      else{
        val +=11*pow(16,0);
      }
      break;
      case 'b':
      if(i==0){
        val = 11*pow(16,1);
      }
      else{
        val +=11*pow(16,0);
      }
      break;
      case 'C':
      if(i==0){
        val = 12*pow(16,1);
      }
      else{
        val +=12*pow(16,0);
      }
      break;
      case 'c':
      if(i==0){
        val = 12*pow(16,1);
      }
      else{
        val +=12*pow(16,0);
      }
      break;
      case 'D':
      if(i==0){
        val = 13*pow(16,1);
      }
      else{
        val +=13*pow(16,0);
      }
      break;
      case 'd':
      if(i==0){
        val = 13*pow(16,1);
      }
      else{
        val +=13*pow(16,0);
      }
      break;
      case 'E':
      if(i==0){
        val = 14*pow(16,1);
      }
      else{
        val +=14*pow(16,0);
      }
      break;
      case 'e':
      if(i==0){
        val = 14*pow(16,1);
      }
      else{
        val +=14*pow(16,0);
      }
      break;
      case 'F':
      if(i==0){
        val = 15*pow(16,1);
      }
      else{
        val +=15*pow(16,0);
      }
      break;
      case 'f':
      if(i==0){
        val = 15*pow(16,1);
      }
      else{
        val +=15*pow(16,0);
      }
      break;
     }
      
     }
     rgbint[j] = val;
  }

}
