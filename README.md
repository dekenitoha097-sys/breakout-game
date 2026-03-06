# Breakout Game 🎮

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![Raylib](https://img.shields.io/badge/Raylib-5.0-green.svg)](https://www.raylib.com/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-orange.svg)](https://github.com/)

Un jeu de casse-briques classique avec éditeur de niveaux intégré, développé en C++ avec la bibliothèque graphique [Raylib](https://www.raylib.com/).

## 📋 Table des Matières

- [Fonctionnalités](#fonctionnalités)
- [Captures d'écran](#captures-décran)
- [Installation](#installation)
  - [Prérequis](#prérequis)
  - [Windows](#windows)
  - [Linux](#linux)
- [Comment Jouer](#comment-jouer)
- [Éditeur de Niveaux](#éditeur-de-niveaux)
- [Structure du Projet](#structure-du-projet)
- [Configuration](#configuration)
- [Crédits](#crédits)
- [Licence](#licence)

## ✨ Fonctionnalités

### Jeu Principal
- 🎯 **Gameplay classique** - Détruisez les briques avec une balle rebondissante
- 🏆 **Système de niveaux** - 30 niveaux disponibles avec progression
- ❤️ **Système de vies** - 3 vies par partie
- 📊 **Sauvegarde automatique** - Progression et paramètres conservés entre les sessions
- 🔊 **Effets sonores** - Sons d'impact et de pickup

### Types de Briques
| Type | Description | Couleur |
|------|-------------|---------|
| 1-Hit Brick | Se détruit en un coup | Rouge |
| 2-Hit Brick | Se détruit en deux coups | Jaune |
| 3-Hit Brick | Se détruit en trois coups | Vert |
| Metal Brick | Incassable | Gris |
| Clear | Case vide | Gris clair |

### Bonus (Power-ups)
- ⚪ **Big Ball** - Double la taille de la balle
- 📏 **Long Paddle** - Double la longueur de la raquette
- 🔮 **Multi-Ball** - Fait apparaître 3 balles

### Éditeur de Niveaux
- 🎨 **Interface intuitive** - Créez vos propres niveaux
- 💾 **Format JSON** - Niveaux enregistrés en JSON
- 🗑️ **5 types de briques** - Sélection facile via l'interface

### Paramètres
- 🔈 **Volume adjustable** - Contrôle du volume principal
- 🔇 **Mode muet** - Couper les sons
- ↩️ **Réinitialisation** - Remettre à zéro la progression

## 📷 Captures d'écran

*(Ajoutez vos captures d'écran ici)*

```
┌─────────────────────────────────────────┐
│           BREAKOUT GAME                 │
│                                         │
│   ┌─────────────────────────────┐       │
│   │     ████  ████  ████  ████  │       │
│   │     ████  ████  ████  ████  │       │
│   │     ████  ████  ████  ████  │       │
│   │                               │       │
│   │            ●                 │       │
│   │                               │       │
│   │      ═════════════          │       │
│   │                               │       │
│   │  ❤️ ❤️ ❤️        SCORE: 0    │       │
│   └─────────────────────────────┘       │
│                                         │
│      [ PLAY ]  [ SETTINGS ]             │
└─────────────────────────────────────────┘
```

## 🚀 Installation

### Prérequis

- **Compilateur C++** (GCC, MinGW, ou Clang)
- **Raylib 5.0** ou supérieur
- **Make** (pour utiliser le Makefile)

### Windows

1. **Installez Raylib pour Windows**
   
   Téléchargez et installez [raylib w64devkit](https://github.com/raysan5/raylib/releases), ou utilisez vcpkg:
   ```bash
   vcpkg install raylib:x64-windows
   ```

2. **Clonez le projet**
   ```bash
   git clone https://github.com/votre-username/breakout-game.git
   cd breakout-game
   ```

3. **Compilez avec Make**
   ```bash
   cd breakout-game
   make
   ```

4. **Lancez le jeu**
   ```bash
   ./breakout-game.exe
   ```

### Linux

1. **Installez les dépendances**
   
   **Debian/Ubuntu:**
   ```bash
   sudo apt update
   sudo apt install build-essential libraylib-dev
   ```

   **Arch Linux:**
   ```bash
   sudo pacman -S raylib
   ```

   **Fedora:**
   ```bash
   sudo dnf install raylib-devel
   ```

2. **Clonez et compilez**
   ```bash
   git clone https://github.com/votre-username/breakout-game.git
   cd breakout-game/breakout-game
   make
   ```

3. **Lancez le jeu**
   ```bash
   ./breakout-game
   ```

## 🎮 Comment Jouer

### Contrôles

| Touche | Action |
|--------|--------|
| ← → ou A D | Déplacer la raquette |
| Espace | Lancer la balle |
| Échap | Mettre en pause |
| P | Revenir au menu |

### Objectif

Détruisez toutes les briques pour passer au niveau suivant. attention aux briques grises (métal) qui sont incassables!

### Conseils

- Attrapez les bonus qui tombent pour obtenir des avantages
- Utilisez les murs pour des rebonds stratégiques
- Gardez vos vies pour les niveaux difficiles

## 🛠️ Éditeur de Niveaux

L'éditeur de niveaux intégré vous permet de créer vos propres niveaux.

### Lancer l'éditeur

```bash
cd breakout-game
# Compilez d'abord si nécessaire
make
# puis lancez l'éditeur
./editor
```

### Utilisation

1. **Sélectionnez un type de brique** dans le panneau de droite
2. **Dessinez sur la grille** en maintenant le clic gauche
3. **Nommez votre niveau** dans le champ de texte
4. **Cliquez SAVE** pour sauvegarder

Les niveaux sont enregistrés dans le dossier `levels/` au format JSON.

### Format JSON des Niveaux

```json
{
    "bricks": [
        {
            "b": {"x": 0, "y": 0, "width": 30, "height": 15},
            "color": {"r": 255, "g": 0, "b": 0, "a": 255},
            "numbers_of_lives": 1,
            "brick_type": "SINGLE_HIT",
            "power_type": "BIG_BALL",
            "isActive": true
        }
    ]
}
```

## 📁 Structure du Projet

```
breakout-game/
├── config/
│   └── game_config.json      # Configuration du jeu
├── font/
│   └── cmu.ttf              # Police utilisée
├── include/
│   ├── breakout.hpp         # En-têtes principaux
│   ├── editor.hpp           # En-têtes de l'éditeur
│   ├── json.hpp             # Bibliothèque JSON
│   └── raylib.h             # En-têtes Raylib
├── obj/
│   └── *.o                  # Fichiers objets compilés
├── resources/
│   ├── Hit1.wav             # Son d'impact
│   └── Pickup8.wav          # Son de bonus
├── src/
│   ├── main.cpp             # Point d'entrée du jeu
│   ├── breakout.cpp         # Logique principale
│   ├── frames.cpp           # Gestion des écrans
│   ├── json.cpp             # Utilitaires JSON
│   └── editor/
│       ├── main.cpp         # Point d'entrée éditeur
│       └── editor.cpp       # Logique de l'éditeur
├── makefile                 # Script de compilation
└── breakout-game.exe        # Exécutable (Windows)
```

## ⚙️ Configuration

Le fichier `config/game_config.json` contient les paramètres du jeu:

```json
{
    "highest_unlocked_level": 3,
    "master_volume": 1.0,
    "muted": false
}
```

| Paramètre | Description | Valeur par défaut |
|-----------|-------------|-------------------|
| `highest_unlocked_level` | Plus haut niveau débloqué | 3 |
| `master_volume` | Volume principal (0.0 - 1.0) | 1.0 |
| `muted` | Mode muet | false |

## 🤝 Contribuer

Les contributions sont les bienvenues! Pour contribuer:

1. Fork le projet
2. Créez une branche (`git checkout -b feature/amelioration`)
3. Commit vos changements (`git commit -am 'Ajout de nouvelles fonctionnalités'`)
4. Push vers la branche (`git push origin feature/amelioration`)
5. Ouvrez une Pull Request

## 📝 Notes Techniques

- **Langage**: C++17
- **Graphique**: Raylib 5.0
- **Format de données**: JSON (nlohmann/json)
- **Cible**: Windows et Linux

### Compilation manuelle (Windows)

```bash
g++ src/*.cpp -o breakout-game -I include -L C:\raylib\w64devkit\lib -lraylib -lopengl32 -lgdi32 -lwinmm
```

### Compilation manuelle (Linux)

```bash
g++ src/*.cpp -o breakout-game -I include -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

## 🙏 Crédits

- **Raylib** - [https://www.raylib.com/](https://www.raylib.com/) - Bibliothèque graphique
- **nlohmann/json** - [https://github.com/nlohmann/json](https://github.com/nlohmann/json) - Parser JSON

## 📄 Licence

Ce projet est sous licence MIT. Voir le fichier [LICENSE](LICENSE) pour plus de détails.

---

<div align="center">

F etait avec ❤️ ☕

</div>
