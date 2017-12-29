#ifndef UNABIZ_ARDUINO_STATE_H
#define UNABIZ_ARDUINO_STATE_H

static const uint8_t stepStart = 1;
static const uint8_t stepListen = 2;
static const uint8_t stepSend = 3;
static const uint8_t stepReceive = 4;
static const uint8_t stepTimeout = 5;
static const uint8_t stepEnd = 6;

static const uint8_t stepSuccess = 101;
static const uint8_t stepFailure = 102;

struct FunctionState {
  //  Represents the state of a function.  Remembers the current step and the success/failure status.
  String functionName = "";
  FunctionState *parentState = 0;
  FunctionState *childState = 0;
  uint8_t currentStep = 0;
  uint8_t nextStep = 0;
  uint32_t delay = 0;
  int int1 = 0;
  unsigned long ulong1 = 0;
};

class State {
public:
  State() {
    init();
  }

  ~State() {
    destroy();
  }

  void destroy() {
    if (!currentState) return;
    FunctionState *childState = currentState->childState;
    if (childState) {
      delete childState;
      currentState->childState = 0;
    }
    delete currentState;
    currentState = 0;
  }

  void init() {
    //  Construct a root state.
    destroy();
    currentState = new FunctionState();
    currentState->functionName = "root";
  }

  uint8_t begin(const String &functionName, uint8_t firstStep) {
    //  If we don't have a child state for this function name, create a new state and reset to the first step.
    FunctionState *childState = currentState->childState;
    if (!childState || childState->functionName != functionName) {
      log2(F("New state"), functionName);
      if (childState) delete childState;
      childState = new FunctionState();
      childState->functionName = functionName;
      childState->currentStep = firstStep;
      childState->parentState = currentState;
      currentState->childState = childState;
      currentState = childState;
    } else {
      //  Else switch to the state and return the last step for this function.
      log2(F("Found state"), functionName);
      currentState = childState;
    }
    return currentState->currentStep;
  }

  void popState(uint8_t currentStep0, uint8_t nextStep0 = 0) {
    if (!currentState) return;
    currentState->currentStep = currentStep0;
    currentState->nextStep = nextStep0;
    currentState = currentState->parentState;
    log2(F("Pop state"), currentState->functionName);
  }

  bool end() {
    //  The function has finished with a success status.
    popState(stepSuccess);
    return true;
  }

  bool endWithFailure() {
    //  The function has finished with a failure status.
    popState(stepFailure);
    return false;
  }

  bool suspend() {
    //  Pop the current state.
    return suspend(currentState->nextStep);
  }

  bool suspend(uint8_t nextStep, uint32_t delay = 0) {
    //  Remember the next step and pop the current state.
    //  TODO: Delay
    popState(currentState->currentStep, nextStep);
    currentState->delay = delay;
    transitionState();
    if (currentState->currentStep == stepFailure) return false;
    return true;
  }

  bool transitionState() {
    //  TODO: If child state has completed, delete child state and move to next step.
    //  Return true if we have transitioned.
    FunctionState *childState = currentState->childState;
    if (!childState) return false;
    if (childState->currentStep != stepSuccess && childState->currentStep != stepFailure) return false;
    if (childState->currentStep == stepFailure) currentState->currentStep = stepFailure;
    else currentState->currentStep = currentState->nextStep;
    delete childState;
    currentState->childState = 0;
    return true;
  }

  void setState(int int1, unsigned long ulong1) { currentState->int1 = int1; currentState->ulong1 = ulong1; }
  void getState(int &int1, unsigned long &ulong1) { int1 = currentState->int1; ulong1 = currentState->ulong1; }

private:
  FunctionState *currentState = 0;
};

#endif // UNABIZ_ARDUINO_STATE_H
