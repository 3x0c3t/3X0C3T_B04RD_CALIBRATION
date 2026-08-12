# 3X0C3T_B04RD_CALIBRATION

Calibration tactile pour **3x0c3t BO4RD** basée sur :

* ESP8266 NodeMCU
* écran TFT SPI 2.8"
* contrôleur graphique ILI9341
* contrôleur tactile XPT2046
* bibliothèque TFT_eSPI
* stockage de la calibration dans SPIFFS

---

## 1. Présentation

`3X0C3T_B04RD_CALIBRATION` est un programme indépendant destiné à effectuer la calibration du tactile XPT2046 utilisé avec le projet **3x0c3t BO4RD**.

Le programme a été volontairement séparé du projet principal afin de disposer d'un outil simple permettant de :

1. initialiser le TFT ;
2. initialiser le tactile ;
3. lancer la calibration tactile ;
4. récupérer les paramètres de calibration ;
5. sauvegarder ces paramètres dans SPIFFS ;
6. recharger automatiquement la calibration lors des démarrages suivants.

```text
                  3x0c3t BO4RD
                       |
                       v
              +------------------+
              |     ESP8266      |
              |    NodeMCU       |
              +--------+---------+
                       |
             +---------+---------+
             |                   |
             v                   v
      +-------------+     +-------------+
      | ILI9341 TFT |     |  XPT2046    |
      | 240 x 320   |     |   TOUCH     |
      +-------------+     +-------------+
             |                   |
             +---------+---------+
                       |
                       v
                 Calibration
                       |
                       v
                    SPIFFS
```

---

# 2. Objectif

Le tactile résistif XPT2046 fournit des coordonnées brutes qui doivent être transformées en coordonnées correspondant à l'écran.

Sans calibration :

```text
TACTILE
   |
   | coordonnées RAW
   v
+-----------------------+
| X=xxxx   Y=xxxx       |
|                       |
|       DECALE          |
|          /            |
|       /               |
|    /                  |
+-----------------------+
        TFT
```

Après calibration :

```text
TACTILE
   |
   | coordonnées RAW
   v
+-----------------------+
|                       |
|     X,Y correctement  |
|       convertis       |
|                       |
|          +            |
|        TOUCH          |
|                       |
+-----------------------+
        TFT 240x320
```

La calibration permet donc de faire correspondre :

```text
COORDONNEES TACTILES
        |
        v
   CALIBRATION
        |
        v
COORDONNEES ECRAN
        |
        v
     X = 0..239
     Y = 0..319
```

---

# 3. Matériel

## ESP8266

Carte utilisée :

```text
+--------------------------------+
|        NodeMCU ESP8266         |
|                                |
|   D0  D1  D2  D3  D4  D5 ...  |
|                                |
|             ESP8266            |
|                                |
+--------------------------------+
```

Carte Arduino :

```text
NodeMCU 1.0 (ESP-12E Module)
```

---

# 4. Écran TFT

Écran :

```text
+------------------------+
|                        |
|        ILI9341         |
|                        |
|                        |
|                        |
|                        |
|                        |
|                        |
+------------------------+

       240 x 320 px
```

Résolution utilisée :

```text
Largeur  : 240 pixels
Hauteur  : 320 pixels
```

---

# 5. Tactile

Contrôleur :

```text
XPT2046
```

Le XPT2046 communique avec l'ESP8266 en SPI.

```text
+-------------+
|   XPT2046   |
|             |
|  X   Y      |
|  Z1  Z2     |
|             |
+------+------+
       |
       | SPI
       |
       v
+-------------+
|   ESP8266   |
+-------------+
```

---

# 6. Architecture logicielle

Le programme est séparé en plusieurs fichiers.

```text
CALIBRATION/
│
├── CALIBRATION.ino
│
├── settings.h
│
├── display.h
├── display.cpp
│
├── touch.h
├── touch.cpp
│
├── calibration.h
└── calibration.cpp
```

Architecture :

```text
                 CALIBRATION.ino
                       |
          +------------+------------+
          |            |            |
          v            v            v
       display       touch      calibration
          |            |            |
          |            |            |
          v            v            v
        TFT         XPT2046       SPIFFS
```

---

# 7. Rôle des fichiers

## CALIBRATION.ino

Point d'entrée du programme Arduino.

Responsabilités :

```text
setup()
   |
   +--> Initialisation série
   |
   +--> Initialisation TFT
   |
   +--> Initialisation tactile
   |
   +--> Calibration si nécessaire
   |
   +--> Programme principal
```

---

## settings.h

Centralise les paramètres du projet.

On y retrouve notamment :

```text
- résolution de l'écran
- broches
- couleurs
- fichier de calibration
- paramètres de fonctionnement
```

Cela permet d'éviter de disperser les constantes dans tous les fichiers.

---

## display.h / display.cpp

Gestion de l'affichage TFT.

Responsabilités :

```text
+-----------------------+
|       DISPLAY         |
+-----------------------+
|                       |
| titres                |
| textes                |
| points calibration    |
| résultats             |
| messages              |
|                       |
+-----------------------+
```

---

## touch.h / touch.cpp

Gestion du tactile.

Responsabilités :

```text
+-------------------------+
|         TOUCH           |
+-------------------------+
|                         |
| initialisation          |
| chargement calibration  |
| calibration             |
| sauvegarde              |
| activation              |
|                         |
+-------------------------+
```

---

## calibration.h / calibration.cpp

Gestion des fonctions spécifiques à la calibration.

Flux :

```text
        START
          |
          v
   Initialisation TFT
          |
          v
   Initialisation TOUCH
          |
          v
   Calibration nécessaire ?
       /          \
     OUI           NON
      |             |
      v             v
 Calibration    Charger fichier
      |             |
      +------ + ----+
             |
             v
       Activer touch
             |
             v
           READY
```

---

# 8. Calibration

Lorsqu'une calibration est nécessaire, le TFT affiche les indications permettant de toucher les points demandés.

```text
+------------------------+
|                        |
|     CALIBRATION        |
|                        |
|  Touchez les points    |
|                        |
|  +                     |
|                        |
|                        |
|                        |
|                        |
+------------------------+
```

Le programme utilise la fonction de calibration de `TFT_eSPI`.

À la fin, cinq valeurs sont obtenues :

```text
CAL_DATA[0]
CAL_DATA[1]
CAL_DATA[2]
CAL_DATA[3]
CAL_DATA[4]
```

Exemple obtenu pendant les tests :

```text
CAL_DATA[0] = 289
CAL_DATA[1] = 3403
CAL_DATA[2] = 403
CAL_DATA[3] = 3413
CAL_DATA[4] = 2
```

Ces valeurs doivent être considérées comme un ensemble de paramètres de calibration et non comme de simples coordonnées `X MIN`, `X MAX`, `Y MIN`, `Y MAX`.

---

# 9. Sauvegarde SPIFFS

La calibration est sauvegardée dans la mémoire flash de l'ESP8266 via SPIFFS.

```text
+-----------------------+
|       ESP8266         |
|                       |
|       FLASH           |
|         |             |
|         v             |
|      SPIFFS           |
|         |             |
|         v             |
| /TouchCalData         |
|                       |
+-----------------------+
```

Fichier utilisé :

```text
/TouchCalData
```

Au démarrage :

```text
              START
                |
                v
         Initialisation
                |
                v
        SPIFFS disponible ?
           /          \
         NON           OUI
          |             |
          v             v
       erreur      fichier présent ?
                         /    \
                       NON     OUI
                        |       |
                        v       v
                   calibrer   charger
```

---

# 10. Calibration forcée

Le projet permet de forcer une nouvelle calibration.

Lorsque la calibration forcée est activée :

```text
START
  |
  v
TFT
  |
  v
TOUCH
  |
  v
CALIBRATION
  |
  v
SAUVEGARDE
  |
  v
READY
```

C'est utile lorsque :

* l'écran tactile a été remplacé ;
* le contrôleur tactile a changé ;
* le montage mécanique a changé ;
* l'orientation de l'écran a changé ;
* les coordonnées sont incorrectes ;
* une ancienne calibration n'est plus adaptée.

---

# 11. Calibration automatique au démarrage

Lorsque la calibration forcée est désactivée :

```text
START
  |
  v
SPIFFS
  |
  +---- /TouchCalData existe ?
  |             |
  |             +---- OUI
  |                    |
  |                    v
  |              Charger données
  |                    |
  |                    v
  |              tft.setTouch()
  |                    |
  |                    v
  |                  READY
  |
  +---- NON
         |
         v
     CALIBRATION
         |
         v
      SAUVEGARDE
         |
         v
        READY
```

Cela évite de recalibrer le tactile à chaque démarrage.

---

# 12. Sortie du moniteur série

Le programme utilise le moniteur série pour afficher les informations importantes.

Exemple :

```text
==============================

   3x0c3t BO4RD

   CALIBRATION

==============================

[TFT] Initialisation...
[TFT] OK

[TOUCH] Initialisation...

=== CALIBRATION TOUCH ===
```

Après la calibration :

```text
[TOUCH] Calibration sauvegardee

=== RESULTATS CALIBRATION ===

CAL_DATA[0] = 289
CAL_DATA[1] = 3403
CAL_DATA[2] = 403
CAL_DATA[3] = 3413
CAL_DATA[4] = 2

==============================

[TOUCH] Calibration terminee
```

L'objectif est volontairement de conserver un terminal lisible.

Le programme n'affiche pas en permanence les valeurs RAW du tactile.

---

# 13. Affichage TFT

Pendant la calibration :

```text
+------------------------+
|                        |
|   CALIBRATION TOUCH    |
|                        |
|   Touchez les points   |
|                        |
|                        |
|          +             |
|                        |
|                        |
+------------------------+
```

Après la calibration :

```text
+------------------------+
|                        |
|    CALIBRATION OK      |
|                        |
|       RESULTATS        |
|                        |
| CAL_DATA[0] : 289      |
| CAL_DATA[1] : 3403     |
| CAL_DATA[2] : 403      |
| CAL_DATA[3] : 3413     |
| CAL_DATA[4] : 2        |
|                        |
| DONNEES SAUVEGARDEES   |
|                        |
+------------------------+
```

---

# 14. Communication SPI

Le TFT et le tactile utilisent le bus SPI.

```text
                  ESP8266
                     |
       +-------------+-------------+
       |             |             |
       v             v             v
      SCK           MOSI          MISO
       |             |             |
       +-------------+-------------+
                     |
                  SPI BUS
                     |
             +-------+-------+
             |               |
             v               v
          ILI9341          XPT2046
            TFT             TOUCH
```

Les deux périphériques peuvent partager :

```text
SCK
MOSI
MISO
```

mais disposent de leurs propres lignes CS.

---

# 15. Dépendances

Le projet utilise principalement :

```text
TFT_eSPI
```

Le cœur utilisé pendant le développement :

```text
ESP8266
```

Configuration de développement :

```text
Carte :
NodeMCU 1.0 (ESP-12E Module)

Core ESP8266 :
3.1.2
```

---

# 16. Installation

## 1. Installer Arduino IDE

Installer Arduino IDE puis ajouter le support ESP8266.

## 2. Installer TFT_eSPI

Installer :

```text
TFT_eSPI
```

depuis le gestionnaire de bibliothèques Arduino.

## 3. Configurer TFT_eSPI

Le fichier de configuration de TFT_eSPI doit correspondre au matériel utilisé.

```text
ESP8266
   |
   +---- SPI
          |
          +---- ILI9341
          |
          +---- XPT2046
```

Une mauvaise configuration TFT_eSPI peut provoquer des erreurs qui n'ont absolument rien à voir avec le code de calibration. Parce que les bibliothèques Arduino aiment parfois faire porter la faute au mauvais endroit.

---

# 17. Compilation

Ouvrir :

```text
CALIBRATION.ino
```

Sélectionner :

```text
NodeMCU 1.0 (ESP-12E Module)
```

Puis compiler et téléverser.

---

# 18. Premier démarrage

Au premier démarrage :

```text
ESP8266
   |
   v
TFT INIT
   |
   v
TOUCH INIT
   |
   v
SPIFFS
   |
   v
PAS DE CALIBRATION
   |
   v
CALIBRATION
```

Le programme demande alors de toucher les points affichés à l'écran.

---

# 19. Démarrages suivants

Une fois la calibration enregistrée :

```text
ESP8266
   |
   v
TFT INIT
   |
   v
TOUCH INIT
   |
   v
SPIFFS
   |
   v
/TouchCalData
   |
   v
CHARGEMENT
   |
   v
TOUCH READY
```

Aucune nouvelle calibration n'est nécessaire.

---

# 20. Refaire une calibration

Pour refaire une calibration :

```text
1. Activer FORCE_CALIBRATION
2. Compiler
3. Téléverser
4. Toucher les points demandés
5. Attendre la sauvegarde
6. Désactiver FORCE_CALIBRATION
7. Recompiler
8. Téléverser
```

---

# 21. Résolution écran

Le projet utilise :

```text
Largeur  = 240 px
Hauteur  = 320 px
```

Repère écran :

```text
             X
             0 --------------------> 239
             |
             |
             |
             |
             |
             |
             |
             v
             Y
            319
```

Coin supérieur gauche :

```text
X = 0
Y = 0
```

Coin inférieur droit :

```text
X = 239
Y = 319
```

---

# 22. Architecture finale

```text
                    +----------------+
                    | CALIBRATION.ino|
                    +-------+--------+
                            |
             +--------------+--------------+
             |              |              |
             v              v              v
       +-----------+  +-----------+  +-------------+
       |  display  |  |   touch   |  | calibration |
       +-----+-----+  +-----+-----+  +------+------+
             |              |               |
             v              v               v
          ILI9341        XPT2046          SPIFFS
             |              |               |
             +--------------+---------------+
                            |
                            v
                    TOUCH CALIBRATED
                            |
                            v
                         240x320
```

---

# 23. Philosophie du projet

Le programme de calibration est volontairement indépendant du programme principal `3x0c3t BO4RD`.

Cela permet de disposer d'un outil dédié :

```text
3x0c3t BO4RD
      |
      +---- CALIBRATION
      |
      +---- QUICK TEST
      |
      +---- KEYPAD
      |
      +---- SD TEST
      |
      +---- TOUCH TEST
      |
      +---- APPLICATION
```

La calibration devient ainsi une étape indépendante du développement de l'interface utilisateur.

---

# 24. État du projet

Version :

```text
v1.0
```

État :

```text
+-----------------------------+
| CALIBRATION                 |
+-----------------------------+
| TFT ILI9341       [ OK ]    |
| XPT2046           [ OK ]    |
| Calibration       [ OK ]    |
| SPIFFS            [ OK ]    |
| Sauvegarde        [ OK ]    |
| Rechargement      [ OK ]    |
+-----------------------------+
```

---

# 25. Structure du dépôt

La racine du dépôt doit rester volontairement simple :

```text
3X0C3T_B04RD_CALIBRATION/
│
├── CALIBRATION.ino
├── calibration.cpp
├── calibration.h
├── display.cpp
├── display.h
├── settings.h
├── touch.cpp
└── touch.h
```

Aucun autre projet Arduino n'est nécessaire dans ce dépôt.

---

# 26. Licence

Projet personnel **3x0c3t BO4RD**.

Copyright © 3x0c3t.

---

# 27. Auteur

**3x0c3t**

Projet :

```text
3x0c3t BO4RD
```

Sous-projet :

```text
3X0C3T_B04RD_CALIBRATION
```

---

# 28. Résumé

```text
        3x0c3t BO4RD
              |
              v
       +--------------+
       | ESP8266       |
       +------+--------+
              |
        +-----+-----+
        |           |
        v           v
     ILI9341     XPT2046
       TFT        TOUCH
        |           |
        +-----+-----+
              |
              v
        CALIBRATION
              |
              v
           SPIFFS
              |
              v
       /TouchCalData
              |
              v
       TOUCH READY
```

**3X0C3T_B04RD_CALIBRATION** fournit ainsi une base indépendante, reproductible et persistante pour calibrer le tactile XPT2046 du système **3x0c3t BO4RD**.
