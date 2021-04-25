# Tiny Ladybug

Applicazione per ATTINY13A.

## Configurazione

Alcuni parametri di funzionamento dell'applicazione sono modificabili con delle macro nel file `main/app_config.h`.
In particolare:
 - `LED_PULSE_STEP` : Incremento del duty cycle del lampeggio del led di stato ogni 100ms (in percentuale).
 - `LED_PULSE_PAUSE`: Tempo di spegnimento del led di stato durante lo stato di attesa (in ms).
 - `STERILIZATION_DURATION`: Durata dello stato di sterilizzazione (in ms).
 - `INPUT_DEBOUNCE`: numero di letture di antirimbalzo relative al contatto della porta (viene fatta una lettura ogni 5ms).

## Compilazione

Il sistema di compilazione si appoggia su [Scons](https://scons.org/). Per compilare il file `.hex` da caricare sul microcontrollore e' necessario avere:
 - Python (2 o 3)
 - Scons
 - `avr-gcc`
 - `avrdude` (opzionale per il caricamento del firmware)

I pacchetti possono essere installati su un sistema basato su Debian con il comando

```
apt install scons gcc-avr avr-libc avrdude
```

Se il sistema e' pronto la compilazione viene portata avanti con il comando `scons`.

Il caricamento del firmware puo' avvenire in diversi modi; il sistema di compilazione utilizza `avrdude` con il comando `scons flash`.

### Docker

Viene fornito un `Dockerfile` che configura un container pronto per la compilazione. Anche senza installare i pacchetti necessari, e' possibile compilare il progetto con i seguenti comandi:

#### Linux

```
docker build -t <tag_immagine> .
docker run --rm -v `pwd`:`pwd` -w `pwd` <tag_immagine> scons
```

#### Windows
```
docker build -t <tag_immagine>
docker run --rm -v ${PWD}:/usr/src/project -w /usr/src/project <tag_immagine> scons
```
