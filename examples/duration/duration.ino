/*
DESCRIPTION
====================
Reports through serial (57600 baud) the time since
a button press (transition from HIGH to LOW).

*/

// Include the Bounce2 library found here :
// https://github.com/thomasfredericks/Bounce-Arduino-Wiring
#include <StandardCplusplus.h>
#include <Bounce2.h>
#include <string>
#include <serstream>
#include <set>

#define BUTTON_PIN 2
#define LED_PIN 13


using namespace std;
namespace std
{
  ohserialstream cout(Serial);
}

const int inputButton[] = {2, 3, 4};
const int countOfInputButtons = sizeof(inputButton)/sizeof(int);
Bounce * debouncedInputButton = new Bounce[countOfInputButtons];


unsigned long buttonPressTimeStamp;

void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // pripojeni tlacitek na debouncer a nastaveni vstupu s pullup rezitorem.
  for (int i = 0; i < countOfInputButtons; i++){
    debouncedInputButton[i].attach(inputButton[i], INPUT_PULLUP);
  }

  // Setup the LED :
  pinMode(LED_PIN,OUTPUT);

}



const long pushedButtonsEvalWindowMillis = 250; // v tomto okne zjistuje jaka tlacitka jsou zmacknuta
unsigned long firstTimeButtonPressed = 0; // cas, kdy prvni tlacitko stisknuto
set<int> pushedButtons; // jaka tlacitka byla v danem okne stisknuta

void loop() {

  unsigned long currentMillis = millis();

  // musime kazdy vstup precist a debouncovat. zapamatujeme si, jaka tlacitka byla zmacknuta
  for (int i = 0; i < countOfInputButtons; i++){
    if (debouncedInputButton[i].update()) {
      cout << "Duration of change: " << debouncedInputButton[i].duration() << "ms" << endl;
      if (debouncedInputButton[i].fell()) {
        if (pushedButtons.empty()) {
          firstTimeButtonPressed = currentMillis;
        }
        pushedButtons.insert(i+1); // at muzeme rozlisit mezi sudym a lichym
        cout << "Button " << i << " was pressed." << endl;
      }
    }
  }

  // pokud bylo nejake tlacitko stisknute a uplynulo casove okno, tak jdeme vyhodnotit, co s tim
  if (!pushedButtons.empty() && currentMillis - firstTimeButtonPressed >= pushedButtonsEvalWindowMillis) {
    // for debug we send what buttons was pressed
    cout << "This buttons was pressed during " << pushedButtonsEvalWindowMillis << "ms window: ";
    for (set<int>::iterator it=pushedButtons.begin(); it!=pushedButtons.end(); ++it) {
      cout << ", " << *it;
    }
    cout << endl;
    // -- enddebug
    pushedButtons.clear();
  }

}

