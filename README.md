# **Projet Arduino - Contrôle de Servomoteur avec Joystick et Feedback Visuel**

## 📌 **Description**

Ce projet Arduino permet de contrôler un **servomoteur** à l'aide d'un **joystick analogique**, tout en affichant les informations sur un **écran LCD RGB Grove**. Le système intègre :

- 📊 **Lecture des entrées** (joystick, capteur de pression)
- 🎮 **Contrôle précis du servomoteur** (0° à 180°)
- 📟 **Affichage des données** (angle, force appliquée, symboles directionnels)
- ⚠️ **Détection d'obstacle** via un capteur de pression

---

## 🛠️ **Matériel Requis**

| Composant                      | Lien                                                             |
| ------------------------------ | ---------------------------------------------------------------- |
| Carte Arduino (Uno/Nano)       | [Lien](https://store.arduino.cc/)                                |
| Grove LCD RGB Backlight (16x2) | [Lien](https://www.seeedstudio.com/Grove-LCD-RGB-Backlight.html) |
| Joystick analogique            | [Lien](https://www.seeedstudio.com/Grove-Thumb-Joystick.html)    |
| Servomoteur (SG90)             | [Lien](https://www.seeedstudio.com/Grove-Servo.html)             |
| Capteur de pression (FSR)      | [Lien](https://www.seeedstudio.com/Grove-FSR.html)               |

---

## 🚀 **Fonctionnalités**

### **1. Contrôle du Servomoteur**

- **Joystick** :
  - ↑ : Augmente l'angle (par pas de 5°)
  - ↓ : Diminue l'angle
  - Affichage en temps réel sur LCD (`0°` à `180°`)

### **2. Feedback Visuel (LCD)**

- **Symboles directionnels** personnalisés :
  - ↑ / ↓ / ← / → / • (centre) / ★ (alerte)
- **Affichage numérique** :
  - Angle actuel (ex: `45degre`)
  - Force de pression (ex: `12newton`)

### **3. Détection d'Obstacle**

- **Capteur de pression** :
  - Si `force ≥ 15newton` → **alerte visuelle** (★) + arrêt du servomoteur
  - Fond LCD passe au **vert** en mode alerte

---

## 🔌 **Branchements**

| Arduino | Composant           | Broche  |
| ------- | ------------------- | ------- |
| `A0`    | Capteur de pression | Signal  |
| `A1`    | Joystick (Axe X)    | Signal  |
| `A2`    | Joystick (Axe Y)    | Signal  |
| `D4`    | Servomoteur         | Signal  |
| `I2C`   | LCD RGB             | SDA/SCL |

---

## ⚙️ **Structure du Code**

```cpp
// 1. Déclarations
struct AnalogBranches { ... };       // Broches analogiques
struct DigitalBranches { ... };      // Broches digitales
struct PreviousMillis { ... };       // Gestion des délais
struct Symbols { ... };              // Caractères personnalisés
struct Data { ... };                 // Variables globales

// 2. Classes
class JoyStickReader { ... };        // Lecture du joystick

// 3. Fonctions principales
void setup() { ... }                 // Initialisation
void loop() { ... }                  // Boucle principale
void toggleBlinkScreen() { ... }     // Gestion LED LCD
```

---

## 🌟 **Symboles Personnalisés**

| Symbole | Code                | Usage                    |
| ------- | ------------------- | ------------------------ |
| **↑**   | `byte arrowUp[8]`   | Joystick vers le haut    |
| **↓**   | `byte arrowDown[8]` | Joystick vers le bas     |
| **★**   | `byte star[8]`      | Alerte (force excessive) |
| **•**   | `byte centerDot[8]` | Position neutre          |

---

## 📊 **Exemple de Sortie LCD**

```
↑ 45degre       (Angle actuel)
★ 18newton      (Alerte force)
```

---

## 🔧 **Personnalisation**

1. **Modifier les seuils** :

   - `data.maxForcePressureSensorOnObject` (défaut: `15newton`)
   - `data.servoStep` (défaut: `5°`)

2. **Ajouter des symboles** :
   - Éditez `struct Symbols` avec [LCD Character Creator](https://maxpromer.github.io/LCD-Character-Creator/).

---

## 📜 **Licence**

MIT License - Libre pour tout usage (personnel/professionnel).

**Auteur** : obed
