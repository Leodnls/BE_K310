# Fiche Technique Complète — Carte NUCLEO-N657XO-Q

> **Objectif :** Comprendre et maîtriser la carte NUCLEO-N657XO-Q, du débutant à l'ingénieur.

---

## Table des matières

1. [Présentation générale de la carte](#1-présentation-générale-de-la-carte)
2. [Architecture du microcontrôleur](#2-architecture-du-microcontrôleur)
3. [Composants principaux de la carte](#3-composants-principaux-de-la-carte)
4. [Fonctionnement global d'un programme STM32](#4-fonctionnement-global-dun-programme-stm32)
5. [Exemples simples expliqués](#5-exemples-simples-expliqués)
6. [Outils de développement](#6-outils-de-développement)
7. [Méthodologie pour développer un projet](#7-méthodologie-pour-développer-un-projet)
8. [Pièges et erreurs fréquentes](#8-pièges-et-erreurs-fréquentes)
9. [Lien avec des projets avancés](#9-lien-avec-des-projets-avancés)

---

## 1. Présentation générale de la carte

### 1.1 Rôle d'un microcontrôleur

Un **microcontrôleur** (MCU) est un circuit intégré qui regroupe sur une seule puce :

- un **processeur** (CPU) pour exécuter des instructions
- de la **mémoire** (Flash pour le code, RAM pour les données)
- des **périphériques** (GPIO, UART, SPI, I2C, ADC, timers, etc.)

> **Analogie :** C'est comme un mini-ordinateur tout-en-un, mais optimisé pour contrôler du matériel en temps réel avec une consommation très faible.

Contrairement à un processeur d'ordinateur, un microcontrôleur :
- démarre instantanément (pas de système d'exploitation complexe)
- réagit en microsecondes à des événements hardware
- fonctionne souvent sur batterie ou avec très peu d'énergie

---

### 1.2 Positionnement de la carte NUCLEO-N657XO-Q

| Caractéristique | Valeur |
|---|---|
| **Microcontrôleur** | STM32N657X0H3Q (famille STM32N6) |
| **Cœur CPU** | ARM Cortex-M55 (ARMv8.1-M) |
| **Fréquence max** | 800 MHz |
| **Flash interne** | Aucune pour le code applicatif (uniquement OTP — mémoire une seule fois programmable pour la configuration et les clés de sécurité) |
| **RAM interne** | 4 Mo (SRAM) |
| **Flash externe** | via XSPI (Octo-SPI) |
| **Connecteur** | ST Zio + morpho (compatible Arduino) |
| **Alimentation** | 3,3 V (via USB ou externe) |
| **Débogueur embarqué** | ST-LINK v3 intégré |
| **Particularité** | NPU (Neural Processing Unit) embarqué |

**Positionnement dans la gamme STM32 :**

```
STM32F0 / G0   → entrée de gamme (Cortex-M0/M0+), simple, basse conso
STM32F4 / G4   → milieu de gamme (Cortex-M4/M33), bonne performance
STM32H7        → haute performance (Cortex-M7, 480 MHz)
STM32N6  ◄◄◄  → ultra haute performance (Cortex-M55, 800 MHz, NPU, MIPI)
```

---

### 1.3 Cas d'usage typiques

- **Vision par ordinateur embarquée** : reconnaissance d'objets, détection de visages
- **IA embarquée (Edge AI)** : inférence de réseaux de neurones en temps réel
- **Robotique avancée** : contrôle de moteurs + traitement d'image simultané
- **Systèmes de sécurité** : caméras intelligentes, surveillance
- **Interfaces homme-machine avancées** : écrans graphiques haute résolution
- **Traitement du signal** : audio, radar, vibration

---

## 2. Architecture du microcontrôleur

### 2.1 Cœur CPU — ARM Cortex-M55

#### Qu'est-ce que l'ARM Cortex-M55 ?

Le **Cortex-M55** est le cœur processeur le plus puissant de la famille Cortex-M. Il est basé sur l'architecture **ARMv8.1-M**.

**Caractéristiques clés :**
- **Fréquence** : jusqu'à 800 MHz
- **Pipeline** : 5 étages (Fetch → Decode → Execute → Memory → Writeback)
- **Jeu d'instructions** : Thumb-2, M-Profile Vector Extension (MVE = Helium)
- **FPU** : unité flottante matérielle (single et double précision)
- **DSP extensions** : calcul de signaux numériques accéléré
- **Helium (MVE)** : calcul vectoriel SIMD (traitement de plusieurs données en parallèle)

#### Comment fonctionne un CPU ?

```
1. FETCH   → lire l'instruction en mémoire
2. DECODE  → comprendre ce que l'instruction veut faire
3. EXECUTE → effectuer l'opération (ALU, FPU, etc.)
4. MEMORY  → lire/écrire en RAM si nécessaire
5. WRITE   → écrire le résultat dans un registre
```

> **Analogie :** Un processeur c'est comme un cuisinier qui lit une recette (fetch), la comprend (decode), prépare les ingrédients (execute), les stocke temporairement (memory) et met le plat sur la table (write).

---

### 2.2 Mémoire

#### Structure mémoire du STM32N6

```
Adresse haute
┌─────────────────────────────────┐ 0xFFFFFFFF
│         Périphériques           │ (Registres hardware)
├─────────────────────────────────┤ 0x40000000
│         SRAM (4 Mo)             │ ← données et pile
├─────────────────────────────────┤ 0x20000000
│    Flash OTP / Boot ROM         │ ← code de démarrage
├─────────────────────────────────┤ 0x08000000
│         (Flash externe XSPI)    │ ← code applicatif
└─────────────────────────────────┤ 0x70000000
Adresse basse
```

**Types de mémoire :**

| Type | Rôle | Taille STM32N6 | Caractéristique |
|---|---|---|---|
| **Flash externe** | Stockage du code | Jusqu'à 512 Mo via XSPI | Non-volatile, lente |
| **SRAM** | Données en cours | 4 Mo | Volatile, très rapide |
| **Cache I** | Accélère lectures code | 32 Ko | Transparent |
| **Cache D** | Accélère lectures data | 32 Ko | Transparent |
| **TCM** | Accès RAM ultra-rapide | Dans SRAM | 0 wait-state |
| **OTP** | Données permanentes | Quelques Ko | Programmable 1 fois |

#### Mémoire Cache

> **Pourquoi le cache est-il nécessaire ?**  
> Le CPU à 800 MHz est beaucoup plus rapide que la mémoire Flash externe. Sans cache, le CPU attendrait la mémoire à chaque instruction.  
> Le cache conserve en mémoire rapide les données récemment utilisées.

**Fonctionnement :**
- Cache hit : donnée trouvée dans le cache → accès instantané
- Cache miss : donnée non trouvée → lecture depuis Flash + mise en cache

---

### 2.3 Bus internes

Le STM32N6 utilise une **matrice de bus AXI/AHB/APB** :

```
CPU (M55)
   │
   ├─── AXI Bus (haute performance) ──→ SRAM, DMA, Flash
   │
   ├─── AHB Bus (moyenne vitesse)  ──→ GPIO, DMA controllers
   │
   └─── APB Bus (basse vitesse)    ──→ UART, SPI, I2C, ADC, Timers
```

**Hiérarchie des bus :**
- **AXI (Advanced eXtensible Interface)** : bus principal, 64-bit, hautes performances
- **AHB (Advanced High-performance Bus)** : bus système, bonne vitesse
- **APB (Advanced Peripheral Bus)** : bus périphériques lents, économe en énergie

> **Important :** La fréquence des périphériques APB est un diviseur de la fréquence CPU. Si CPU = 800 MHz et diviseur APB = 4, alors les périphériques APB tournent à 200 MHz.

---

### 2.4 Système d'horloges (Clock System)

#### Pourquoi les horloges sont-elles cruciales ?

Tout dans un microcontrôleur est **synchronisé par une horloge**. Sans horloge, rien ne fonctionne. Les horloges déterminent :
- la vitesse d'exécution du CPU
- la vitesse des communications (UART baud rate, SPI clock, etc.)
- la précision des timers

#### Sources d'horloge disponibles

| Source | Fréquence | Usage |
|---|---|---|
| **HSI** (High Speed Internal) | 64 MHz | Démarrage, usage général |
| **HSE** (High Speed External) | 4–48 MHz | Quartz externe, haute précision |
| **LSI** (Low Speed Internal) | 32 kHz | Watchdog, RTC basse consommation |
| **LSE** (Low Speed External) | 32,768 kHz | RTC horloge temps réel |
| **PLL** (Phase Locked Loop) | Jusqu'à 800 MHz | Multiplication des fréquences |

#### Comment la PLL multiplie la fréquence ?

```
HSI (64 MHz)
    │
    ▼
  ÷ M (prédiviseur)   → par ex. ÷ 4 = 16 MHz
    │
    ▼
  × N (multiplicateur) → par ex. × 50 = 800 MHz
    │
    ▼
  ÷ P/Q/R (post-div)  → divise pour chaque bus
    │
    ├──→ CPU : 800 MHz
    ├──→ AHB : 400 MHz
    └──→ APB : 200 MHz
```

> **Astuce :** STM32CubeMX génère automatiquement la configuration des horloges avec un outil graphique. Il vous suffit de définir la fréquence souhaitée et il calcule les diviseurs.

---

## 3. Composants principaux de la carte

### 3.1 GPIO — General Purpose Input/Output

#### À quoi ça sert ?

Les GPIO sont les **broches multifonctions** du microcontrôleur. Ce sont vos points de contact avec le monde extérieur : LED, boutons, capteurs, moteurs, etc.

#### Modes de fonctionnement

| Mode | Description | Exemple |
|---|---|---|
| **Output Push-Pull** | Pilote 0 ou 3,3 V | Allumer une LED |
| **Output Open-Drain** | Tire vers 0 V ou laisse flottant | Bus I2C |
| **Input Float** | Lit l'état sans résistance | Bouton externe avec résistance |
| **Input Pull-Up** | Résistance interne vers 3,3 V | Bouton actif bas |
| **Input Pull-Down** | Résistance interne vers GND | Bouton actif haut |
| **Analog** | Connecté à l'ADC/DAC | Mesure de tension |
| **Alternate Function** | Attribué à un périphérique | UART_TX, SPI_CLK, etc. |

#### Comment l'utiliser concrètement ?

```c
// Configuration via HAL (STM32CubeMX génère ça automatiquement)
GPIO_InitTypeDef GPIO_InitStruct = {0};

__HAL_RCC_GPIOA_CLK_ENABLE();          // Activer l'horloge du port A

GPIO_InitStruct.Pin   = GPIO_PIN_5;    // Broche PA5
GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  // Sortie push-pull
GPIO_InitStruct.Pull  = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

// Utilisation
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   // Mettre à 1 (3,3 V)
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // Mettre à 0 (GND)
HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);                // Inverser l'état
```

> **Important :** Toujours activer l'horloge du port GPIO avant de le configurer (`__HAL_RCC_GPIOx_CLK_ENABLE()`). C'est une erreur très fréquente chez les débutants.

---

### 3.2 Timers

#### À quoi ça sert ?

Les timers sont des **compteurs matériels** indépendants du CPU. Ils permettent de :
- générer des délais précis
- créer des signaux PWM (pour servo-moteurs, LED dimmable)
- mesurer des durées (capture)
- déclencher des événements à intervalles réguliers

#### Types de timers sur STM32N6

| Type | Résolution | Canaux | Usage |
|---|---|---|---|
| **TIM avancé** (TIM1, TIM8) | 16-bit | 4 + 4 comp. | PWM moteur, contrôle avancé |
| **TIM général** (TIM2–TIM5) | 32-bit | 4 | Usage polyvalent |
| **TIM basique** (TIM6, TIM7) | 16-bit | 0 | Déclencher DAC, délais |
| **LPTIM** | 16-bit | 1 | Mode basse consommation |
| **SysTick** | 24-bit | — | Base de temps HAL (1 ms) |

#### Fonctionnement simple d'un timer

```
Horloge APB (ex: 200 MHz)
    │
    ▼
  Prescaler (÷ valeur) → réduit la fréquence de comptage
    │
    ▼
  Compteur (0 → ARR)   → compte de 0 jusqu'à la valeur Auto-Reload Register
    │
    ▼
  Overflow (événement) → déclenche une interruption ou une action
```

**Exemple : générer une interruption toutes les 1 ms avec TIM6**
```
Horloge TIM = 200 MHz
Prescaler   = 199  → fréquence comptage = 200 MHz / (199+1) = 1 MHz
ARR         = 999  → interruption = 1 MHz / (999+1) = 1 kHz = 1 ms
```

#### PWM — Pulse Width Modulation

Le **PWM** crée un signal carré dont on fait varier le rapport cyclique (duty cycle).

```
100% duty cycle: ████████████████  → LED pleine lumière / servo 180°
 50% duty cycle: ████████________  → LED à moitié / servo 90°
 25% duty cycle: ████____________  → LED à 1/4 / servo 45°
```

**Utilisation concrète :**
```c
// Démarrer PWM sur TIM1, canal 1
HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

// Modifier le duty cycle (0 à ARR)
__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty_value);
```

---

### 3.3 UART / USART — Communication série

#### À quoi ça sert ?

L'UART (**Universal Asynchronous Receiver Transmitter**) est le protocole de communication série le plus simple. Il permet d'échanger des données avec :
- un PC (via terminal série, pour le debug)
- un module GPS, Bluetooth, WiFi
- un autre microcontrôleur

#### Fonctionnement

```
MCU                           PC / autre appareil
TX ────────────────────────→ RX
RX ←──────────────────────── TX
GND ─────────────────────── GND
```

- **Asynchrone** : pas de signal d'horloge partagé
- **Baud rate** : vitesse en bits/seconde (ex: 115200 bauds = 115 200 bits/s)
- **Format de trame** : 1 start bit + 8 bits data + 1 stop bit

```
_____┐     ┌──┐     ┌──────┐  ┌─ ...
     │start│B0│...  │ B7   │stop
     └─────┘  └─────┘      └──┘
```

**Utilisation concrète :**
```c
uint8_t msg[] = "Hello STM32!\r\n";

// Envoyer des données (mode bloquant)
HAL_UART_Transmit(&huart1, msg, sizeof(msg), HAL_MAX_DELAY);

// Recevoir 1 octet
uint8_t rx_byte;
HAL_UART_Receive(&huart1, &rx_byte, 1, HAL_MAX_DELAY);
```

---

### 3.4 SPI — Serial Peripheral Interface

#### À quoi ça sert ?

Le SPI est un protocole **synchrone** (avec horloge) en **full-duplex** pour communiquer avec des capteurs, des écrans, des mémoires Flash externes.

**Signaux SPI :**
| Signal | Direction | Rôle |
|---|---|---|
| **SCK** | Master → Slave | Horloge |
| **MOSI** | Master → Slave | Données vers esclave |
| **MISO** | Slave → Master | Données depuis esclave |
| **CS/NSS** | Master → Slave | Sélection de l'esclave |

> **Avantage vs UART :** Beaucoup plus rapide (jusqu'à plusieurs dizaines de MHz).  
> **Inconvénient :** Nécessite une broche CS par esclave.

```c
// Envoyer/recevoir simultanément
uint8_t tx_data[] = {0x01, 0x02};
uint8_t rx_data[2];

HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET); // CS = 0 (actif)
HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2, HAL_MAX_DELAY);
HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);   // CS = 1
```

---

### 3.5 I2C — Inter-Integrated Circuit

#### À quoi ça sert ?

L'I2C est un protocole **synchrone** sur **seulement 2 fils**, permettant de connecter plusieurs esclaves sur le même bus. Idéal pour les capteurs (température, accéléromètre, etc.).

**Signaux I2C :**
| Signal | Rôle |
|---|---|
| **SCL** | Horloge (Serial Clock) |
| **SDA** | Données (Serial Data) — bidirectionnel |

```
MCU (master)         Capteur A    Capteur B    Capteur C
    SCL ────────────────┬────────────┬────────────┘
    SDA ────────────────┘────────────┘
```

- Chaque esclave a une **adresse unique** sur 7 bits (0x00 à 0x7F)
- Vitesse standard : 100 kHz / Fast mode : 400 kHz / Fast+ : 1 MHz

```c
uint8_t data[2];
// L'adresse I2C 7 bits (0x68) est décalée d'1 bit pour le format HAL
// (le bit de poids faible R/W est géré automatiquement par la bibliothèque HAL)
uint16_t dev_addr = 0x68 << 1; // → 0xD0 : adresse 8 bits utilisée par HAL

// Lire 2 octets depuis le registre 0x3B en une seule transaction atomique
// HAL_I2C_Mem_Read est préférable : il effectue l'écriture du registre
// puis la lecture sans relâcher le bus (repeated start).
HAL_I2C_Mem_Read(&hi2c1, dev_addr, 0x3B, I2C_MEMADD_SIZE_8BIT,
                 data, 2, HAL_MAX_DELAY);
```

---

### 3.6 ADC — Analog to Digital Converter

#### À quoi ça sert ?

L'ADC convertit une **tension analogique** (continue) en **valeur numérique**.  
Indispensable pour lire des capteurs analogiques : potentiomètre, capteur de lumière, température, microphone, etc.

**Paramètres clés :**
- **Résolution** : 12 bits → valeurs de 0 à 4095 (2^12 - 1)
- **Tension de référence (VREF)** : 3,3 V par défaut
- **Formule** : `Tension = (Valeur_ADC / 4095) × VREF`

**Exemple :** ADC lit 2048 → Tension = (2048/4095) × 3,3 V ≈ 1,65 V

```c
// Démarrer une conversion ADC
HAL_ADC_Start(&hadc1);
HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

// Lire la valeur brute
uint32_t raw = HAL_ADC_GetValue(&hadc1);

// Convertir en tension (mV)
float tension_mV = (raw * 3300.0f) / 4095.0f;
```

---

### 3.7 DAC — Digital to Analog Converter

#### À quoi ça sert ?

Le DAC fait l'**inverse de l'ADC** : convertit une valeur numérique en tension analogique.  
Utilisé pour : génération de signaux audio, pilotage de moteurs analogiques, références de tension.

```c
// Générer une tension de 1,65 V (moitié de 3,3 V)
uint32_t value = 2048; // 12 bits → 0 à 4095
HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
```

---

### 3.8 Interface MIPI CSI-2 (Caméra)

#### À quoi ça sert ?

La NUCLEO-N657XO-Q intègre une interface **MIPI CSI-2** pour connecter des modules caméra haute résolution.

**MIPI CSI-2 (Camera Serial Interface 2)** :
- Interface industrielle standard pour capteurs d'images
- Communication différentielle LVDS à très haute vitesse
- Supporte plusieurs voies de données (1, 2, ou 4 lanes)
- Bitrate par lane : jusqu'à plusieurs Gbps
- Compatible avec la majorité des capteurs CMOS modernes (OmniVision, Sony, etc.)

**Pipeline caméra sur STM32N6 :**
```
Capteur MIPI CSI-2
    │ (flux pixels bruts)
    ▼
DCMIPP (Digital Camera Interface and Pixel Pipeline)
    │ (redimensionnement, conversion de format)
    ▼
DMA vers SRAM / PSRAM
    │
    ▼
ISP (Image Signal Processor) — traitement d'image
    │
    ▼
NPU / CPU — inférence IA ou affichage
```

---

### 3.9 NPU — Neural Processing Unit

#### À quoi ça sert ?

Le **NPU** est un **accélérateur matériel dédié à l'inférence de réseaux de neurones**. Il permet d'exécuter des modèles IA (CNN, MobileNet, etc.) à très haute vitesse et avec une consommation réduite.

**Caractéristiques du NPU du STM32N6 :**
- Performances : jusqu'à **600 GOP/s** (milliards d'opérations par seconde)
- Types d'opérations : convolutions, pooling, activation, etc.
- Quantification : supporte INT8, INT16 (modèles quantifiés)
- Outil associé : **ST Edge AI** (anciennement X-CUBE-AI)

**Chaîne de traitement IA :**
```
Modèle TensorFlow/PyTorch (sur PC)
    │
    ▼
Quantification INT8 (STM32Cube.AI / ST Edge AI)
    │
    ▼
Code C généré pour STM32N6 + NPU
    │
    ▼
Inférence en temps réel sur la carte
```

> **Exemple de performances :** Détection d'objets MobileNet V1 en ~10 ms sur NPU vs ~100 ms sur CPU seul.

---

### 3.10 DMA — Direct Memory Access

#### À quoi ça sert ?

Le DMA permet de **transférer des données en mémoire sans utiliser le CPU**. Ainsi, le CPU peut faire d'autres calculs pendant que le DMA transfère les données.

**Exemple sans DMA (bloquant) :**
```
CPU : envoie octet 1 → attend → envoie octet 2 → attend → ...
(CPU bloqué pendant tout le transfert)
```

**Exemple avec DMA :**
```
CPU : configure le DMA, puis continue son code
DMA : transfère tous les octets en fond
DMA : génère une interruption quand c'est terminé
```

```c
// UART avec DMA (non-bloquant)
uint8_t buffer[] = "Message long...\r\n";
HAL_UART_Transmit_DMA(&huart1, buffer, sizeof(buffer));
// CPU libre immédiatement, interruption quand terminé
```

---

## 4. Fonctionnement global d'un programme STM32

### 4.1 Démarrage du microcontrôleur

```
Mise sous tension
    │
    ▼
Vecteur de reset (adresse 0x08000004)
    │
    ▼
Startup code (startup_stm32n6xxxx.s)
  - Initialise la pile (SP)
  - Copie .data de Flash vers RAM
  - Met .bss à zéro
  - Appelle SystemInit()
    │
    ▼
SystemInit() — configure les horloges de base
    │
    ▼
main()
```

### 4.2 Structure type d'un firmware

```c
int main(void)
{
    /* 1. Initialisation matérielle */
    HAL_Init();                // Initialise la bibliothèque HAL (SysTick 1ms)
    SystemClock_Config();      // Configure les PLLs et horloges
    
    /* 2. Initialisation des périphériques */
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_TIM1_Init();
    MX_ADC1_Init();
    
    /* 3. Démarrage des services */
    HAL_TIM_Base_Start_IT(&htim1);  // Démarrer timer avec interruptions
    
    /* 4. Boucle principale (infinie) */
    while (1)
    {
        // Traitement principal
        process_data();
        
        // Peut inclure un sleep pour économiser de l'énergie
        // HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    }
}
```

### 4.3 Le système d'interruptions

#### Pourquoi les interruptions ?

> **Analogie :** Sans interruptions, le CPU devrait vérifier en permanence si quelque chose s'est passé (polling). C'est comme si vous regardiez votre téléphone toutes les secondes pour voir si vous avez un message.  
> Avec les interruptions, c'est votre téléphone qui vous sonne quand il y a un message (vous pouvez faire autre chose entre-temps).

**Fonctionnement :**
```
CPU exécute main()
    │
    │ [Événement : bouton appuyé, timer overflow, UART reçu...]
    │
    ▼ (CPU sauvegarde son contexte automatiquement)
ISR (Interrupt Service Routine)
    - Code rapide et court
    - Ne jamais bloquer dans une ISR
    │
    ▼ (CPU restaure son contexte)
Reprise de main()
```

**Exemple d'ISR pour timer :**
```c
// Appelé automatiquement toutes les 1 ms (si TIM6 configuré ainsi)
void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim6); // Appelle le callback HAL
}

// Callback utilisateur appelé par HAL
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        // Code exécuté toutes les 1 ms
        toggle_led();
    }
}
```

#### NVIC — Nested Vectored Interrupt Controller

Le **NVIC** gère les priorités des interruptions :
- Chaque IRQ a une **priorité** (0 = plus haute, 15 = plus basse sur 4 bits)
- Une IRQ de haute priorité peut interrompre une ISR de faible priorité
- `HAL_Init()` configure SysTick avec la priorité la plus basse

```c
// Configurer la priorité d'une IRQ
HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 5, 0); // groupe 5, sous-groupe 0
HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
```

### 4.4 Gestion du temps

**Trois approches selon le besoin :**

| Méthode | Précision | CPU bloqué ? | Usage |
|---|---|---|---|
| `HAL_Delay(ms)` | ~1 ms | Oui | Debug, délais simples |
| Timer + interruption | Très précise | Non | Production, temps réel |
| DWT cycle counter | Nanoseconde | Non | Mesure de performances |

```c
// Méthode 1 : délai bloquant (à éviter en production)
HAL_Delay(500); // attend 500 ms

// Méthode 2 : timer non-bloquant
uint32_t last_tick = HAL_GetTick();
while (1) {
    if (HAL_GetTick() - last_tick >= 500) {
        last_tick = HAL_GetTick();
        // Action toutes les 500 ms
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }
    // Autre code s'exécute normalement
}

// Méthode 3 : DWT (cycle counter)
CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
DWT->CYCCNT = 0;
DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
uint32_t start = DWT->CYCCNT;
// ... code à mesurer ...
uint32_t cycles = DWT->CYCCNT - start;
float time_us = cycles / (SystemCoreClock / 1e6f);
```

---

## 5. Exemples simples expliqués

### 5.1 Faire clignoter une LED (version simple avec delay)

**Matériel :** LED sur PA5 (broche LD2 de la NUCLEO)

```c
#include "main.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init(); // Configure PA5 en sortie push-pull

    while (1)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   // LED ON
        HAL_Delay(500);                                        // Attend 500 ms
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // LED OFF
        HAL_Delay(500);                                        // Attend 500 ms
    }
}
```

**Problème avec cette version :**
- Le CPU est **bloqué** pendant les 500 ms de chaque `HAL_Delay()`
- Impossible de faire autre chose en même temps

---

### 5.2 Faire clignoter une LED (version améliorée sans delay)

**Principe :** Utiliser `HAL_GetTick()` pour mesurer le temps écoulé.

```c
#include "main.h"

#define BLINK_PERIOD_MS 500

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    uint32_t last_toggle = HAL_GetTick();

    while (1)
    {
        /* Clignotement LED sans bloquer */
        if (HAL_GetTick() - last_toggle >= BLINK_PERIOD_MS)
        {
            last_toggle = HAL_GetTick();
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        }

        /* Ici on peut faire d'autres choses pendant ce temps */
        process_sensors();
        update_display();
    }
}
```

**Avantages :**
- CPU libre entre les bascules
- Facile d'ajouter d'autres tâches indépendantes
- Base d'un scheduler coopératif simple

---

### 5.3 Version avec interruption de timer (la meilleure approche)

**Configuration dans CubeMX :**
1. Activer TIM6 → Enable
2. Prescaler = 799, Period = 999 → interruption toutes les 1 ms  
   *(en supposant une horloge TIM à 800 MHz : 800 MHz / (799+1) = 1 MHz, puis 1 MHz / (999+1) = 1 kHz = 1 ms)*
3. Activer l'interruption globale TIM6

```c
/* Dans main.c */
HAL_TIM_Base_Start_IT(&htim6); // Démarrer le timer avec interruptions

/* Callback appelé toutes les X ms par l'interruption */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint32_t count = 0;
    if (htim->Instance == TIM6)
    {
        count++;
        if (count >= 500) // Toutes les 500 ms
        {
            count = 0;
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // Basculer la LED
        }
    }
}

/* La boucle principale est totalement libre */
while (1)
{
    // Tout autre traitement
}
```

---

## 6. Outils de développement

### 6.1 STM32CubeMX

**Rôle :** Outil graphique de **configuration du microcontrôleur** et de **génération de code**.

**Ce qu'il fait :**
- Visualise et configure les broches GPIO (pinout)
- Configure les horloges (Clock Tree visuel)
- Configure les périphériques (UART, SPI, I2C, ADC, etc.)
- Génère un projet complet (HAL + middleware) pour CubeIDE

**Workflow CubeMX :**
```
1. Choisir le MCU (STM32N657X0HxQ)
2. Assigner les pins (GPIO, UART_TX, UART_RX, SPI_CLK, etc.)
3. Configurer les périphériques (baud rate, résolution ADC, etc.)
4. Configurer les horloges (Clock Tree)
5. Générer le code → projet CubeIDE
```

---

### 6.2 STM32CubeIDE

**Rôle :** IDE complet basé sur Eclipse pour **écrire, compiler et flasher** le firmware.

**Fonctionnalités clés :**
- Éditeur C/C++ avec autocomplétion
- Compilateur **arm-none-eabi-gcc** intégré
- Débogueur **GDB + ST-LINK** intégré
- Visualisation des registres en temps réel
- Profiling et analyse mémoire

**Structure d'un projet généré :**
```
MonProjet/
├── Core/
│   ├── Inc/          ← Fichiers .h (déclarations)
│   │   ├── main.h
│   │   └── stm32n6xx_hal_conf.h
│   └── Src/          ← Fichiers .c (implémentation)
│       ├── main.c    ← Point d'entrée (votre code ici)
│       ├── stm32n6xx_it.c   ← Gestionnaires d'interruptions
│       └── system_stm32n6xx.c
├── Drivers/
│   ├── CMSIS/        ← Couche bas niveau ARM
│   └── STM32N6xx_HAL_Driver/  ← Bibliothèques HAL ST
├── MonProjet.ioc     ← Fichier de configuration CubeMX
└── STM32N657...FLASH.ld ← Script de linker (carte mémoire)
```

---

### 6.3 Compilation et flashage

#### Compilation

```
Code source (.c / .h)
    │
    ▼
Préprocesseur → expansion des macros (#define, #include)
    │
    ▼
Compilateur (arm-none-eabi-gcc) → code assembleur
    │
    ▼
Assembleur → fichiers objets (.o)
    │
    ▼
Linker (ld) → binaire ELF (.elf) selon le script .ld
    │
    ▼
objcopy → fichier .hex / .bin (image mémoire)
```

#### Flashage

**Via ST-LINK intégré (CubeIDE) :**
1. Connecter la carte via USB
2. Dans CubeIDE : `Run → Debug` ou `Run → Run`
3. Le ST-LINK transfère le .hex dans la Flash via SWD (Serial Wire Debug)

> **Important pour STM32N6 :** La carte N6 n'a pas de Flash interne. Le code est stocké dans une **Flash externe XSPI**. Le **FSBL (First Stage Boot Loader)** doit être chargé en premier pour initialiser la Flash externe.

**Via STM32CubeProgrammer (ligne de commande) :**
```bash
STM32_Programmer_CLI -c port=SWD -d mon_firmware.hex -rst
```

#### Débogage avec CubeIDE

**Fonctionnalités essentielles :**
- **Breakpoints** : arrêter l'exécution à une ligne précise
- **Step Over / Into / Out** : avancer instruction par instruction
- **Watch expressions** : surveiller la valeur d'une variable en temps réel
- **Memory view** : voir le contenu de la RAM/Flash
- **Peripheral registers** : voir l'état de tous les registres hardware
- **Live expressions** : mettre à jour les variables sans arrêter le CPU

---

## 7. Méthodologie pour développer un projet

### 7.1 Comment démarrer un projet

**Étape 1 : Définir les besoins**
- Quels capteurs/actionneurs ?
- Quels protocoles de communication ?
- Contraintes temps réel ?
- Consommation électrique ?

**Étape 2 : Schéma de connexion**
- Dessiner les connexions entre la carte et les composants externes
- Vérifier les niveaux logiques (3,3 V vs 5 V)
- Identifier les broches disponibles sur la NUCLEO

**Étape 3 : Configuration CubeMX**
- Ouvrir CubeMX, sélectionner STM32N657X0HxQ
- Assigner les périphériques aux broches
- Configurer les horloges
- Générer le projet

**Étape 4 : Développement incrémental**
- Commencer par le minimum (blink LED = "Hello World" embarqué)
- Ajouter un périphérique à la fois
- Tester chaque ajout avant de continuer

---

### 7.2 Organisation du code

**Bonne pratique : séparer les responsabilités**

```
Core/Src/
├── main.c              ← Initialisation + boucle principale
├── app_tasks.c         ← Logique applicative
├── sensors.c           ← Gestion des capteurs
├── motor_control.c     ← Contrôle moteurs
└── communication.c     ← UART, BLE, etc.

Core/Inc/
├── app_tasks.h
├── sensors.h
├── motor_control.h
└── communication.h
```

**Règles importantes :**
- **Ne jamais modifier** les fichiers générés par CubeMX hors des zones `/* USER CODE BEGIN */` / `/* USER CODE END */`
- Mettre son code **uniquement entre ces balises** pour éviter qu'il soit écrasé lors d'une re-génération

```c
/* USER CODE BEGIN 0 */
#include "sensors.h"
uint32_t my_variable = 0;
/* USER CODE END 0 */
```

---

### 7.3 Comment tester et déboguer

**Approche progressive :**

1. **Printf sur UART** (le plus simple)
```c
// Redirection de printf vers UART (ajouter dans main.c)
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

// Utilisation
printf("ADC value: %lu\r\n", adc_val);
printf("Temperature: %.2f C\r\n", temperature);
```

2. **LED de debug** : faire clignoter une LED selon l'état du programme

3. **Breakpoints dans CubeIDE** : arrêter le programme et inspecter les variables

4. **Live Expressions** : surveiller des variables en temps réel sans arrêter

5. **Logic Analyzer** : brancher un analyseur logique sur les signaux SPI/I2C/UART

---

## 8. Pièges et erreurs fréquentes

### 8.1 Erreurs de débutant

| Erreur | Symptôme | Solution |
|---|---|---|
| Oublier `__HAL_RCC_GPIOx_CLK_ENABLE()` | GPIO ne fonctionne pas | Toujours activer l'horloge avant d'utiliser un périphérique |
| Modifier du code hors des balises USER CODE | Code effacé à la re-génération CubeMX | Mettre son code entre `USER CODE BEGIN` et `USER CODE END` |
| `HAL_Delay()` dans une ISR | Programme bloqué | Ne jamais utiliser HAL_Delay dans une interruption |
| Variable partagée entre ISR et main sans `volatile` | Valeur jamais mise à jour | Déclarer la variable partagée comme `volatile` |
| Adresse I2C incorrecte | `HAL_I2C_Master_Transmit` retourne ERROR | Vérifier l'adresse dans la datasheet + décalage de 1 bit |
| Baud rate UART incorrect | Données corrompues | Vérifier que les deux côtés ont le même baud rate |

---

### 8.2 Erreurs hardware

| Erreur | Conséquence | Solution |
|---|---|---|
| Broche 5 V connectée à une GPIO 3,3 V | Destruction du MCU | Utiliser un level shifter ou vérifier la tolérance |
| Court-circuit sur l'alimentation | Carte qui ne démarre pas | Toujours vérifier les connexions avant de mettre sous tension |
| Résistances pull-up manquantes sur I2C | Communication I2C instable | Ajouter 4,7 kΩ sur SDA et SCL vers VCC |
| Condensateurs de découplage manquants | Perturbations sur alimentation | Placer 100 nF près de chaque broche VCC du MCU |
| SWD mal connecté | Impossible de flasher | Vérifier SWDIO, SWDCLK, GND et RST |

---

### 8.3 Erreurs de logique

| Erreur | Description | Solution |
|---|---|---|
| Race condition | Deux parties du code modifient la même variable | Utiliser `__disable_irq()` / `__enable_irq()` ou des flags atomiques |
| Stack overflow | Programme plante aléatoirement | Augmenter la taille de la stack dans le linker script |
| Débordement d'entier | `uint8_t count` dépasse 255 | Choisir le bon type (`uint16_t`, `uint32_t`) |
| Division entière | `3 / 2 = 1` en C | Utiliser des flottants : `3.0f / 2` ou multiplier avant de diviser |
| `while` infini sans WFI | Consommation inutile | Utiliser `__WFI()` en attente pour économiser l'énergie |

---

## 9. Lien avec des projets avancés

### 9.1 Vision embarquée

**Pipeline complet sur NUCLEO-N657XO-Q :**

```
Module caméra (OV5640 via MIPI CSI-2)
    │
    ▼
DCMIPP (capture + redimensionnement)
    │ Par ex. 640×480 → 224×224 RGB888
    ▼
SRAM (buffer image)
    │
    ▼
Prétraitement (normalisation, conversion de format)
    │
    ▼
NPU (inférence modèle CNN)
    │
    ▼
Résultat : label + score de confiance
    │
    ▼
Affichage (LTDC → écran) ou transmission (UART → PC)
```

**Outils :**
- **ST Edge AI** : convertit un modèle TensorFlow/ONNX en code C optimisé NPU
- **STM32N6 AI Ecosystem** : exemples de référence pour la détection d'objets
- **OpenMV** : portage Python pour STM32 (prototypage rapide)

---

### 9.2 IA embarquée (Edge AI)

**Workflow complet :**

```
1. Collecte de données (PC)
2. Entraînement modèle (TensorFlow / PyTorch)
3. Quantification INT8 (TensorFlow Lite)
4. Conversion ST Edge AI → code C
5. Intégration dans projet STM32CubeIDE
6. Test et optimisation (mémoire, latence)
7. Déploiement sur carte
```

**Modèles courants pour edge :**
- **MobileNet V1/V2** : classification d'images
- **SSD MobileNet** : détection d'objets
- **YOLO Nano** : détection d'objets légère
- **DS-CNN** : reconnaissance de mots-clés audio

**Métriques à surveiller :**
- **Latence** : temps d'inférence (cible < 100 ms pour temps réel)
- **Mémoire** : poids du modèle en Flash, activations en RAM
- **Précision** : impact de la quantification INT8 sur les performances

---

### 9.3 Communication capteurs avancée

**Architecture typique d'un nœud IoT avec STM32N6 :**

```
Capteurs analogiques (ADC)
    Température, pression, lumière...
    
Capteurs numériques (I2C/SPI)
    IMU (accéléromètre + gyroscope)
    Capteur de gaz
    Capteur de distance ToF (VL53L5CX)
    
Traitement STM32N6
    Fusion de données capteurs
    Filtre de Kalman (CMSIS-DSP)
    Inférence NPU
    
Communication
    UART → Module WiFi (ESP32)
    SPI  → Module LoRa (SX1276)
    I2C  → Module BLE
    USB  → Hôte PC
```

**Bibliothèques utiles :**
- **CMSIS-DSP** : calcul mathématique optimisé (FFT, filtres, matrices)
- **FreeRTOS** : système temps réel pour gérer plusieurs tâches
- **LwIP** : pile TCP/IP légère pour Ethernet
- **TouchGFX** : framework interface graphique haute performance

---

## Annexe — Résumé des registres et adresses utiles

### Registres de contrôle GPIO (exemple GPIOA)

| Registre | Adresse | Rôle |
|---|---|---|
| `MODER` | Base + 0x00 | Mode (entrée/sortie/AF/analogique) |
| `OTYPER` | Base + 0x04 | Type de sortie (push-pull/open-drain) |
| `OSPEEDR` | Base + 0x08 | Vitesse de sortie |
| `PUPDR` | Base + 0x0C | Pull-up / Pull-down |
| `IDR` | Base + 0x10 | Lecture de l'état des entrées |
| `ODR` | Base + 0x14 | Écriture de l'état des sorties |
| `BSRR` | Base + 0x18 | Set/Reset atomique des bits |

### Commandes HAL les plus utilisées

```c
/* GPIO */
HAL_GPIO_WritePin(GPIOx, GPIO_PIN_x, GPIO_PIN_SET/RESET);
HAL_GPIO_TogglePin(GPIOx, GPIO_PIN_x);
GPIO_PinState s = HAL_GPIO_ReadPin(GPIOx, GPIO_PIN_x);

/* Délai */
HAL_Delay(ms);
uint32_t t = HAL_GetTick(); // ms depuis démarrage

/* UART */
HAL_UART_Transmit(&huartX, buf, len, timeout);
HAL_UART_Receive(&huartX, buf, len, timeout);
HAL_UART_Transmit_DMA(&huartX, buf, len);

/* I2C */
HAL_I2C_Master_Transmit(&hi2cX, addr, buf, len, timeout);
HAL_I2C_Master_Receive(&hi2cX, addr, buf, len, timeout);
HAL_I2C_Mem_Write(&hi2cX, addr, reg, size, buf, len, timeout);
HAL_I2C_Mem_Read(&hi2cX, addr, reg, size, buf, len, timeout);

/* SPI */
HAL_SPI_Transmit(&hspiX, buf, len, timeout);
HAL_SPI_Receive(&hspiX, buf, len, timeout);
HAL_SPI_TransmitReceive(&hspiX, tx, rx, len, timeout);

/* ADC */
HAL_ADC_Start(&hadcX);
HAL_ADC_PollForConversion(&hadcX, timeout);
uint32_t val = HAL_ADC_GetValue(&hadcX);

/* Timer */
HAL_TIM_Base_Start_IT(&htimX);
HAL_TIM_PWM_Start(&htimX, TIM_CHANNEL_x);
__HAL_TIM_SET_COMPARE(&htimX, TIM_CHANNEL_x, value);
```

---

## Ressources

- 📖 **Datasheet STM32N657** : [st.com → STM32N657](https://www.st.com/en/microcontrollers-microprocessors/stm32n6-series.html)
- 📖 **Reference Manual STM32N6 (RM0456)** : document complet de tous les registres
- 🛠 **STM32CubeMX** : [st.com/stm32cubemx](https://www.st.com/en/development-tools/stm32cubemx.html)
- 🛠 **STM32CubeIDE** : [st.com/stm32cubeide](https://www.st.com/en/development-tools/stm32cubeide.html)
- 🤖 **ST Edge AI** : [st.com/stm32-model-zoo](https://github.com/STMicroelectronics/stm32ai-modelzoo)
- 📚 **STM32N6 HAL Drivers** : dans le dossier `Drivers/` de chaque projet

---

*Fiche rédigée pour la carte NUCLEO-N657XO-Q — STM32N657X0H3Q — Cortex-M55 @ 800 MHz*
