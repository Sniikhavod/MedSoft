#include <TrueRandom.h> //Pour avoir de bien meilleurs chiffres aleatoire, .zip en annexe du rapport
#include <EEPROM.h> //Pour pouvoir ecrire dans la memoire solide de la carte nos donnée de reglage (Mortalite 0/, localisation 0/1, mode d'affichage 0/1/2, stock 0/1)

//(donnée constructeur)
//Definition necessaire au fonctionnement de l'ecran, .zip en annexe du rapport
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h> //Pour les fonctions tactile

//(donnée constructeur)
// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//Elegoo_TFTLCD tft; // Ou  si cela marche pas, utiliser la version longue :
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); //Indispensable avant l'initialisation mais je ne comprend pas bien le fonctionnement derierre
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); //Indispensable mais je ne comprend pas bien (donnée constructeur)

//Pour gagner du temps et en lisibilité, je definis certainnes couleurs pour pouvoir utiliser la variable plustot que leurs codes en Hexadecimal 
#define NOIR         0x0000
#define BLEU         0x001F
#define ROUGE        0xF800
#define VERT         0x07E0
#define CYAN         0x07FF
#define VIOLET       0xF81F
#define JAUNE        0xFFE0
#define BLANC        0xFFFF
#define OLIVE        0x7BE0
#define DARKGREY     0x7BEF
#define LIGHTGREY    0xC618

void setup() {
//Initialisation
  Serial.begin(9600); //Initialisation du port serie
  pinMode(2, INPUT); //Utilisé par le TrueRandom
  Serial.println("Bonjour !"); //Retour de test du port serie
  Serial.println(F("TFT LCD test")); //Retour de test l'ecran lcd
  //Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height()); //Affichage des dimensions de l'ecran
  tft.reset(); //Sans cette commande a cette endroit le code ne fonctionne pas, je ne sais pas pourquoi (donnée constructeur)
  
////********Zone a mettre en commentaire permet d'initialiser et de tester les valeurs dans l'EEPROM
//  bool local = 1; //0 si non, 1 si oui
//  bool mortalite = 1; 
//  bool stock = 0;
//  int modeAff = 2; //0 pour minimaliste, 1 pour le temps et l'action a effectue, 2 pour le temps, l'action et le croquis d'un Homme avec le bobo en qestion --- Cf. Rapport
//  
//  //test de l'ecriture et de lecture dans l'EEPROM
//  byte test= 0;
//  int i = 0;
//  EEPROM.write(i, test);
//  Serial.println(EEPROM.read (i));
//
//   EEPROM.write(1, local); //.Write pour ecrire des bit
//   EEPROM.write(2, mortalite);
//   EEPROM.write(3, stock);
//   EEPROM.put(4, modeAff); //.Put pour ecrire des octets
//   
//  //Test si les valeurs sont correctement ecrite
//Serial.println(EEPROM.read (1));
//Serial.println(EEPROM.read (2));
//Serial.println(EEPROM.read (3));
//int Tampon;
//EEPROM.get(4, Tampon);
//Serial.println(Tampon);
//
////*******************Fin de la zone a mettre en commentaire 

//(donnée constructeur)
uint16_t identifier = tft.readID();//Tout une procedure de code doit etre faite pour determinerquel driver il faut 
tft.begin(0x9341); //C'est la valeur a utiliser pour ma configuration (Arduino Uno R3 et l'ecran tactile ELEGOO)
//J'ai effectuer cette procedure (~40 lignes de code donner par le constructeur)sur un autre code, j'ai recuperer la valeur hexadecimal a utiliser (ici, 0x9341)
//et je l'utilise en l'entrant direcement dans la fonction qui lance l'affichage de l'ecran
tft.begin(0x9341); //C'est la valeur a utiliser pour ma configuration (Arduino Uno R3 et l'ecran tactile ELEGOO)

//valeur pour la pression d'appuis sur l'ecran
  // pressure of 0 means no pressing! //(donnée constructeur)
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Il faut calibré les valeurs suivant, optenues en testant un des programmes donné par le constructeur
// Sans cette calibration, l'ecran n'est pas precis lors de la localisation de l'appuis (pour faire simple, plus de details dans le rapport)
//Touch For New ILI9341 TP
#define TS_MINX 117
#define TS_MAXX 952
#define TS_MINY 133
#define TS_MAXY 902

tft.setRotation(2); //Pour avoir l'ecran dans un sens, 0 = 0°, 1 = 90°, 2 = 180°, 3 = 270°
tft.setCursor(0, 0); //Initialisation du curseur (Pas obligatoire)

} //fin de setup()

void loop() {
//Declaration des variables interne
  //Read(1) pour la variable "local", 2 pour "mortalité", 3 pour "stock",etc --- Cf. Void Setup
  bool local = EEPROM.read (1);//Pour recuperer l'activation de la localisation des degats ou non
  bool mortalite = EEPROM.read (2); //Pour recuperer l'activation de la mortalité ou non
  bool stock = EEPROM.read (3); //Pour recuperer la gestion des stock ou non
  int modeAff = 0; 
  EEPROM.get(4, modeAff); //Pour recuperer le mode d'affichage, ce n'est pas un bit mais un octet, il faut donc crée un integer a l'avance pour la stocker
  
  tft.fillScreen(NOIR); //Mettre le fond en noir sur l'ecran, pour l'economie d'energie
  unsigned long start = micros(); //Visiblement inutle(donnée constructeur), bon pour le moment je garde mais a tester
  
AffichageStartPage(); //Fonction pour dessiner la page principale

//Attente de l'appuis sur le bouton
  bool flag= true;
  while (flag == true){
    TSPoint p = ts.getPoint(); //pour recuperer les coordonées du point d'appuis
    pinMode(XM, OUTPUT); //Ces deux ligne sont importes, se sont des variable partager par plusieurs librairies
    pinMode(YP,OUTPUT);  //Elles doivent etre motifier ici pour que le code fonctionne
  
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE){ //On regarde si il y a un appuis, une pression sur l'ecran
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0); //si c'est le cas, on convertis les coordonée en x et y en pixel
      p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
    
      if(p.x >60 && p.x <180 && p.y > 60 && p.y<260){ //On regarde si la zone d'appuis est dans le rectangle "Cliquez ici"
          Serial.print("Appuis dans le rectangle de start"); //debug lors des test des zones de boutons
          flag= false;
      }
      if(p.x >180 && p.x <240 && p.y > 0 && p.y<50){ //On regarde si la zone d'appuis est dans le rectangle "Reglage"
          Serial.print("Appuis dans le rectangle de reglage"); //debug lors des test des zones de boutons
          Reglage();
          tft.fillScreen(NOIR);// On efface tout
          AffichageStartPage(); //Et on redessine la page principale apres le menu reglage
      }
    }
  }

//if(p.z>200){ //Si il y a un appuis quelquonque (n'impote ou sur l'ecran), 
flag= false; //Alors on sort de la boucle infinie d'attente
pinMode(XM, OUTPUT); //Ces deux ligne sont importes, se sont des variable partager par plusieurs librairies, elles doivent etre motifier ici pour que le code fonctionne
pinMode(YP,OUTPUT);



//Debut du code
  tft.fillScreen(NOIR);
  Diagnostique(); //Pour lancé l'animation de diagnostique
  delay(500); 
  int i = TypeDeSoin(); //TypeDeSoin permet de générer le probleme rencontré (hemrogie, facture, ...) en generant un nombre entre 0 et 100
  if (i <= 10 && i >= 0) {
    Ongle(); 
  }
  if (i <= 30 && i >= 11) {
    Hemoragie(); 
  }
  if (i <= 50 && i >= 31) {
    Luxation();
  }
  if (i <= 70 && i >= 51) {
    Balle(local, mortalite); 
  }
  if (i <= 90 && i >= 71) {
    ForteHemoragie(mortalite);
  }
  if (i <= 100 && i >= 91) {
    Cardiaque(mortalite);
  }

} //fin du Loop

//**********************************************************************************************************************************************************
//************************************************************************************************************************Debut des fonctions
//**********************************************************************************************************************************************************

void Diagnostique() { //Animation de debut de diagnostique
  
  tft.setTextColor(BLANC);
  tft.setTextSize(2);
  tft.setCursor(40,20);
  tft.println("Diagnostic en          cours !");
  tft.println();
  tft.print("|");
  for(int i=0; i<8; i++){
    tft.print("-");
    delay(150);
  }
  for(int i=8; i<14; i++){
    tft.print("-");
    delay(50);
  }
  for(int i=14; i<18; i++){
    tft.print("-");
    delay(100);
  }
  tft.println("|");
}

//**********************************************************************************************************************************************************
//************************************************************************************************************************Zone des fonction de types de soins 
//**********************************************************************************************************************************************************

int TypeDeSoin() {
  int probleme = TrueRandom.random(0, 100); //Changer les valeurs ici pour provoqué qu'un type de soin en particumier
  return probleme;
}



void Ongle() {
  int tempsmin = 4000; //1sec = 1000, donc 4000 = 4 sec
  int tempsmax = 8000;
  int Temps = TrueRandom.random(tempsmin, tempsmax);   //On genere un temps aléatoire entre les bornes diffinies ici       
  tft.println("Le patient souffre  d'un ongle cassee");
  tft.setCursor(0,145);//Mise du curseur a un endroit pour un affichage propre
  tft.println("Bisous magique");
  while(Temps >=0){
    tft.fillRect(0,165,20,20, NOIR);//Affichage d'un carrée noir pour recouvrir le dernier nombre affiché, afin que le suivant soit lisible
    tft.setCursor(0,165);
    tft.println(Temps/1000);//Affichage du nombre courant de temps restant 
    delay(1000);
    Temps=Temps-1000;
  }
  tft.println();
  Vivant();
}



void Hemoragie() {
  int tempsmin = 6000; //Temps minimum d'attente
  int tempsmax = 12000; //Temps maximal d'attente
  int Temps = TrueRandom.random(tempsmin, tempsmax); //On genere un temps aléatoire entre les bornes diffinies ici
  tft.println("Le patient souffre  d'une faible        hemoragie"); //Affichage du probleme
  tft.setCursor(0,165); //Mise du curseur a un endroit pour un affichage propre
  tft.println("Bandez la plais"); //Affichage du texte
  while(Temps >=0){ //Tant qu'il reste du temps alors
    tft.fillRect(0,185,40,20, NOIR); //Affichage d'un carrée noir pour recouvrir le dernier nombre affiché, afin que le suivant soit lisible
    tft.setCursor(0,185); 
    tft.println(Temps/1000);//Affichage du nombre courant de temps restant 
    delay(1000); //On attend 1 seconde
    Temps=Temps-1000; //On soustrait 1 seconde du temps a attendre
  }
  tft.println();
  Vivant(); //Le joueurs ne peux pas sucombé a ce type de soin, donc il vie
}



void Luxation() {
  int tempsmin = 10000;
  int tempsmax = 14000;
  int Temps = TrueRandom.random(tempsmin, tempsmax);
  int ReferenceT = Temps;
  tft.println("Le patient souffre  d'une luxation");
  tft.setCursor(0,145);
  tft.println("Imobilisation");
  while( Temps >= ReferenceT/3*2){ //Ici je prend un tier (Division par 3) car il y a 3 cas, et le multiplie par 2 pour avoir 2/3 du temps de reference
    tft.fillRect(0,165,40,20, NOIR);
    tft.setCursor(0,165);
    tft.println(Temps/1000);     //Si 4 etapes de soin divisé 4 puis x3,  pour le deuxieme divisé par 4 puis x2, ... x1 ...
    delay(1000);
    Temps=Temps-1000;
  }
  tft.fillRect(0,135,240,30, NOIR);
  tft.setCursor(0,145);
  tft.println("Analgesiques");
    while( Temps >= ReferenceT/3*1){ //Ici je prend un tier (Division par 3) car il y a 3 cas, et le multiplie par 1 pour avoir 1/3 du temps de reference
    tft.fillRect(0,165,40,20, NOIR);
    tft.setCursor(0,165);
    tft.println(Temps/1000);
    delay(1000);
    Temps=Temps-1000;
  }
  tft.fillRect(0,135,240,40, NOIR);
  tft.setCursor(0,145);
  tft.println("Attelle");
  while( Temps >= 0){
    tft.fillRect(0,165,40,20, NOIR);
    tft.setCursor(0,165);
    tft.println(Temps/1000); //Ici j'ai le dernier tier du temps de reference
    delay(1000);
    Temps=Temps-1000;
  }
  tft.println();
  Vivant();
}



void Balle(bool local, bool mortalite) {
  int tempsmin = 12000;
  int tempsmax = 16000;
  int gravite = 0; //Augmente ou diminue le delais en fonction de ka gravité de la blessure, 1000 = 1 sec
  int membre = TrueRandom.random(0,5); //Generation d'un aleatoire 
  
//********************************Gestion de la localisation

  if (local == 1){ //Si la localisation des degats est activé
    
    if (membre == 0){ //Si 0 balle dans la tete
       tft.println("Le patient souffre  d'une balle dans la tete"); //Affichage de la zone
       gravite = 7000; //Ajout du delais 
    }
    if (membre == 1){
       tft.println("Le patient souffre  d'une balle dans le buste "); 
       gravite = 5000;
    }
    if (membre == 2){ //Si 1 balle dans le bras gauche
       tft.println("Le patient souffre  d'une balle dans le bras gauche "); //Affichage de la zone
       gravite = 0; //Ajout du delais 
    }
    if (membre == 3){
        tft.println("Le patient souffre  d'une balle dans le bras droit ");
       gravite = 3000;
    }
    if (membre == 4){
        tft.println("Le patient souffre  d'une balle dans la jambe droite ");
       gravite = 2000;
    }
    if (membre == 5){
        tft.println("Le patient souffre  d'une balle dans la jambe gauche "); 
       gravite = 3000; //En moyenne les blessures ont une moyenne de 3300 lors de la localisation 
    }
  }else 
  tft.println("Le patient souffre  d'une impacte de balle "); //Si la localisation des degats n'est pas activée
  gravite = 3000;
  
//********************************Gestion du temps

  int Temps = TrueRandom.random(tempsmin, tempsmax);
  int ReferenceT = Temps + gravite;
  tft.setCursor(0,165);
  tft.println("Imobilisation");
  while( Temps >= ReferenceT/3*2){ 
      tft.fillRect(0,185,40,20, NOIR);
      tft.setCursor(0,185);
      tft.println(Temps/1000);     
      delay(1000);
      Temps=Temps-1000;
    } 
  tft.fillRect(0,155,240,40, NOIR);
  tft.setCursor(0,165);
  tft.println("Analgesiques");
  while( Temps >= ReferenceT/3*1){ 
    tft.fillRect(0,185,40,20, NOIR);
    tft.setCursor(0,185);
    tft.println(Temps/1000);
    delay(1000);
    Temps=Temps-1000;
  }
  tft.fillRect(0,155,240,40, NOIR);
  tft.setCursor(0,165);
  tft.println("Bandage");
  while( Temps >= 0){
    tft.fillRect(0,185,40,20, NOIR);
    tft.setCursor(0,185);
    tft.println(Temps/1000);
    delay(1000);
    Temps=Temps-1000;
  }
  
//********************************Gestion de la mortalitée

  if (mortalite == 0 || local == 0){ //On regarde si soit la localisation ou la mortalité sont sur Non
      tft.println();
      Vivant(); //Ici mortalité a 0, -> si il ne peux pas mourrir, le joueurs vie et retourne en jeux
  }else //Sinon on execute les autre verifications
      if (membre <=1){ //On verifie si il a été toucher a la tete ou au buste
          int mort = TrueRandom.random(0,100); //On genere un pourcentage sur ses chances de survie
          if (mort >= 75){ //Si le pourcatge est superieur a la valeur de mortalité 
               tft.println();
               Mort();   //Le joueur sucombe
          }else //Sinon 
              tft.println();
              Vivant(); //Le joueurs vie
      }else //Sinon qui n'as pas été toucher a la tete ou au buste vie
          tft.println();
          Vivant(); //Si le joueur n'est pas toucher a la tete ou au buste 
      
}//********************************Fin de la fonction



void ForteHemoragie(bool mortalite) {
  int tempsmin = 13000;
  int tempsmax = 19000;
  tft.println("Le patient souffre  d'une forte         hemoragie");
  
//********************************Gestion du temps

  int Temps = TrueRandom.random(tempsmin, tempsmax);
  int ReferenceT = Temps;
  tft.setCursor(0,165);
  tft.println("Garot");
  while( Temps >= ReferenceT/4*3){ 
      tft.fillRect(0,185,40,20, NOIR);
      tft.setCursor(0,185);
      tft.println(Temps/1000);     
      delay(1000);
      Temps=Temps-1000;
    } 
  tft.fillRect(0,155,240,40, NOIR);
  tft.setCursor(0,165);
  tft.println("Injection morphine");
  while( Temps >= ReferenceT/4*2){ 
    tft.fillRect(0,185,40,20, NOIR);
    tft.setCursor(0,185);
    tft.println(Temps/1000);
    delay(1000);
    Temps=Temps-1000;
  }
  tft.fillRect(0,155,240,40, NOIR);
  tft.setCursor(0,165);
  tft.println("Transfusion");
  while( Temps >= ReferenceT/4*1){
    tft.fillRect(0,185,40,20, NOIR);
    tft.setCursor(0,185);
    tft.println(Temps/1000);
    delay(1000);
    Temps=Temps-1000;
  }
  tft.fillRect(0,155,240,40, NOIR);
  tft.setCursor(0,165);
  tft.println("Bandage");
  while( Temps >= 0){
    tft.fillRect(0,185,40,20, NOIR);
    tft.setCursor(0,185);
    tft.println(Temps/1000);
    delay(1000);
    Temps=Temps-1000;
  }
  
//********************************Gestion de la mortalitée

  if (mortalite == 1){ //Si la mort est possible
    int mort = TrueRandom.random(0,100); //On genere un pourcentage
    if (mort >= 75){ //Si le pourcatge généré est superieur a la valeur de mortalité (en % et definis dans les parametres)
      tft.println();
      Mort();   //Le joueur sucombe
    }else{ //Sinon
      tft.println();
      Vivant();} //Le joueurs vie
  }else{ //Sinon
  tft.println();
  Vivant();} //Le joueurs vie
}//********************************Fin de la fonction


void Cardiaque(bool mortalite) {
  int tempsmin = 18000;
  int tempsmax = 22000;
  tft.println("Le patient souffre  d'une crise         cardique ");
  
//********************************Gestion du temps
  int Temps = TrueRandom.random(tempsmin, tempsmax);
  int ReferenceT = Temps;
  tft.setCursor(0,165);
  tft.println("Injection epinephrine");
  while( Temps >= ReferenceT/3*2){ 
      tft.fillRect(0,185,40,20, NOIR);
      tft.setCursor(0,185);
      tft.println(Temps/1000);     
      delay(1000);
      Temps=Temps-1000;
    } 
  tft.fillRect(0,155,240,40, NOIR);
  tft.setCursor(0,165);
  tft.println("Massage cardiaque");
  while( Temps >= ReferenceT/3*1){ 
      tft.fillRect(0,185,40,20, NOIR);
      tft.setCursor(0,185);
      tft.println(Temps/1000);     
      delay(1000);
      Temps=Temps-1000;
    } 
  tft.fillRect(0,155,240,40, NOIR);
  tft.setCursor(0,165);
  tft.println("Defibrilation");
  while( Temps >= 0){ 
      tft.fillRect(0,185,40,20, NOIR);
      tft.setCursor(0,185);
      tft.println(Temps/1000);     
      delay(1000);
      Temps=Temps-1000;
    } 

//********************************Gestion de la mortalitée

  if (mortalite == 1){ //Si la mort est possible
      int mort = TrueRandom.random(0,100); //On genere un pourcentage
      if (mort >= 60){ //Si le pourcatge  est superieur a la valeur de mortalité
          tft.println();
          Mort();   //Le joueur sucombe
      }else{ //Sinon
          tft.println();
          Vivant();} //Le joueurs vie
  }else{ //Si mortalite est a 0
      tft.println();
      Vivant(); }//Le joueurs vie
}

//**********************************************************************************************************************************************************
//************************************************************************************************************************Zone des fonctions de vie ou de mort
//**********************************************************************************************************************************************************

void Vivant(){
  tft.setTextSize(2);
  tft.fillScreen(VERT);
  tft.setTextColor(NOIR);
  tft.setCursor(30,150);
  tft.println("Retour en jeux !");
  delay(5000);
  //tft.fillScreen(NOIR);
}

void Mort(){
  tft.setTextSize(2);
  tft.fillScreen(ROUGE);
  tft.setTextColor(NOIR);
  tft.setCursor(12,130);
  tft.println("Le patient succombe         ...");
  tft.println();
  tft.println("  Retour au spawn !");
  
  //Affichage d'une croix
  tft.fillRect(110,260,10,150, NOIR); //(coordonée X du coin en haut a gauche, coord Y du meme point,longueur, hauteur du rectangle)taille de l'ecran : 240*360p
  tft.fillRect(90,280,50,10, NOIR);

  delay(5000);
}

void AffichageStartPage(){
  tft.fillScreen(NOIR); //Mettre le fond en noir sur l'ecran
//Affichage du titre
  tft.setCursor(20,15);
  tft.setTextColor(BLANC);
  tft.setTextSize(5);
  tft.print("MEDSOFT");
//Affichage du sous titre
  tft.setCursor(95,60);
  tft.setTextColor(BLANC);
  tft.setTextSize(2);
  tft.print("2020");
//Affichage du bouton principal  
  tft.fillRect(60,80,120,180, OLIVE); //(coordonée X du coin en haut a gauche, coord Y du meme point,longueur, hauteur du rectangle)taille de l'ecran : 240*360p
  tft.drawRect(60,80,120,180, BLANC);
  tft.setCursor(80,100);
  tft.setTextColor(BLANC);
  tft.setTextSize(2);
  tft.print("Cliquez");
  tft.setCursor(83,140);
  tft.setTextSize(4);
  tft.print("ici");
//Affichage du bouton de reglage
  tft.fillRect(180,270,60,90,DARKGREY);
  tft.setCursor(190,290);
  tft.setTextColor(BLANC);
  tft.setTextSize(1);
  tft.print("Reglage");
//Affichage de la croix de gauche
  tft.fillRect(5,150,50,10, ROUGE); //(coordonée X du coin en haut a gauche, coord Y du meme point,longueur, hauteur du rectangle)taille de l'ecran : 240*360p
  tft.fillRect(25,130,10,50, ROUGE);
//Affichage de la croix du bas
  tft.fillRect(190,150,50,10, ROUGE); //Axe Horizontal
  tft.fillRect(210,130,10,50, ROUGE);//Axe vertical
//Affichage de la croix de droite
  tft.fillRect(90,285,50,10, ROUGE); 
  tft.fillRect(110,265,10,50, ROUGE); 
  
}


//**********************************************************************************************************************************************************
//************************************************************************************************************************Zone des fonctions de reglage interne
//**********************************************************************************************************************************************************

//Afin d'avoir de nombreuse variable en retour de fonction, le mieux serrait soit d'utiliser des pointeurs soit de ne pas utiliser de fonction et de rajouter ces codes dans le main loop
// Solution retenue : Ecrire dans l'EEPROM directement via la fonction EEPROM.update(),
void Reglage(){
//Zone de reglage
  bool local = EEPROM.read (1); //Read(1) pour la variable "local", 2 pour "mortalité", 3 pour "stock",etc --- Cf. Void Setup 
  bool mortalite = EEPROM.read (2);
  bool stock = EEPROM.read (3);
  tft.fillScreen(NOIR); //Mettre le fond en noir sur l'ecran

//Affichage du premier reglage
  tft.setTextColor(BLANC);
  tft.setTextSize(2);
  tft.setCursor(0,0);
  tft.print("HIT localise ?"); //Localisation ou non
  tft.fillRect(0,30,120,60,BLEU);
  tft.fillRect(120,30,120,60,ROUGE);
  tft.setCursor(40,50);
  tft.println("Oui");
  tft.setCursor(160,50);
  tft.println("Non ");

//Affichage du Deuxieme reglage
  tft.setCursor(0,100);
  tft.print("Mort possible ?");
  tft.fillRect(0,130,120,60,BLEU);
  tft.fillRect(120,130,120,60,ROUGE);
  tft.setCursor(40,150);
  tft.println("Oui");
  tft.setCursor(160,150);
  tft.println("Non ");
  
//Affichage du Troisieme reglage
  tft.setCursor(0,200);
  tft.print("Decompte stock ?");
  tft.fillRect(0,230,120,60,BLEU);
  tft.fillRect(120,230,120,60,ROUGE);
  tft.setCursor(40,250);
  tft.println("Oui");
  tft.setCursor(160,250);
  tft.println("Non ");
//Affichage pour quittez et revenir au debut
  tft.setCursor(0,300);
  tft.println("    -> Quitter <-");

  bool flag= true;
  while (flag == true){
 
    TSPoint p = ts.getPoint(); //pour recuperer les coordonées du point d'appuis
    pinMode(XM, OUTPUT); //Ces deux ligne sont importes, se sont des variable partager par plusieurs librairies, elles doivent etre motifier ici pour que le code fonctionne
    pinMode(YP,OUTPUT);
  
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE){ //On regarde si il y a un appuis
      p.x = map(p.x, TS_MAXX, TS_MINX, tft.width(), 0); //si c'est le cas, on convertis les coordonée en x et y en pixel
      p.y = map(p.y, TS_MAXY, TS_MINY, tft.height(), 0);
    
      if(p.x >120 && p.x <240 && p.y > 10 && p.y<80){ //On regarde si la zone d'appuis est dans le rectangle "Oui" du premier reglage
          Serial.println("Oui du premier reglage"); //debug lors des test des zones de boutons

           tft.fillRect(200,0,35,20,NOIR);
         local=1;
      }
      if(p.x >00 && p.x <120 && p.y > 10 && p.y<80){ //On regarde si la zone d'appuis est dans le rectangle "Non" du premier reglage
         Serial.println("Non du premier reglage");//debug lors des test des zones de boutons
 
          tft.fillRect(200,0,35,20,NOIR);
         local=0;
      }
      if(p.x >120 && p.x <240 && p.y > 110 && p.y<180){ //On regarde si la zone d'appuis est dans le rectangle "Oui" du deuxieme reglage
          Serial.println("Oui du deuxieme reglage");//debug lors des test des zones de boutons
          tft.fillRect(200,100,35,20,NOIR);
          mortalite=1;
      }
      if(p.x >00 && p.x <120 && p.y > 110 && p.y<180){ //On regarde si la zone d'appuis est dans le rectangle "Non" du deuxieme reglage
         Serial.println("Non du deuxieme reglage");//debug lors des test des zones de boutons
         tft.fillRect(200,100,35,20,NOIR);
         mortalite=0;
      }
      if(p.x >120 && p.x <240 && p.y > 220 && p.y<290){ 
          Serial.println("Oui du troisieme reglage");//debug lors des test des zones de boutons
          tft.fillRect(200,200,35,20,NOIR);
          stock=1;
      }
      if(p.x >00 && p.x <120 && p.y > 220 && p.y<290){ 
         Serial.println("Non du troisieme reglage");//debug lors des test des zones de boutons
         tft.fillRect(200,200,35,20,NOIR);
         stock=0;
      }
      if(p.x >00 && p.x <240 && p.y>300){ 
         Serial.println("Sortie des reglages");//debug lors des test des zones de boutons
         EEPROM.update(1, local); //.Write pour ecrire des bit
         EEPROM.update(2, mortalite); //.Update pour ecrire des bit seulement si c'est une nouvelle valeur, afin de ne pas ecrire un 1 si la valeur 
         EEPROM.update(3, stock);                                                                 //actuelle est deja un 1, afin d'economiser l'EEPROM
         flag = false;
      } 
    }//Fin du test de pression
    
   //Maj de l'affichage
  tft.setCursor(200,0);
  if (local == 1){ //0 si non, 1 si oui
    tft.println("Oui");
  }else{
  tft.println("Non ");}
  
  tft.setCursor(200,100);
  if (mortalite == 1){ //0 si non, 1 si oui
    tft.println("Oui");
  }else{
  tft.println("Non ");}
  
  tft.setCursor(200,200);
  if (stock == 1){ //0 si non, 1 si oui
    tft.println("Oui");
  }else{
  tft.println("Non ");}
  }//Fin du while
  
} //fin de la fonction
