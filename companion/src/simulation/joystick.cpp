#include "joystick.h"

Joystick::Joystick(QObject *parent, int joystickEventTimeout, bool doAutoRepeat, int repeatDelay)
  : QObject(parent)
{
  if ( SDL_WasInit(SDL_INIT_JOYSTICK) ) {
    int i;
    for (i = 0; i < SDL_NumJoysticks(); i++)
      joystickNames.append(SDL_JoystickName(i));
    connect(&joystickTimer, SIGNAL(timeout()), this, SLOT(processEvents()));
  } else {
    fprintf(stderr, "ERROR: couldn't initialize SDL joystick support\n");
  }

  joystick = NULL;
  numAxes = numButtons = numHats = numTrackballs = 0;
  autoRepeat = doAutoRepeat;
  autoRepeatDelay = repeatDelay;
  eventTimeout = joystickEventTimeout;
}

Joystick::~Joystick()
{
  if ( isOpen() )
    close();
}

bool Joystick::open(int stick)
{
  if (isOpen())
    close();

  joystick = SDL_JoystickOpen(stick);
  if (joystick) {
    numAxes = SDL_JoystickNumAxes(joystick);
    numButtons = SDL_JoystickNumButtons(joystick);
    numHats = SDL_JoystickNumHats(joystick);
    numTrackballs = SDL_JoystickNumBalls(joystick);
    joystickTimer.start(eventTimeout);
    return true;
  }
  else {
    fprintf(stderr, "ERROR: couldn't open SDL joystick #%d", stick);
    return false;
  }
}

void Joystick::close()
{
  joystickTimer.stop();
  if (joystick) {
    SDL_JoystickClose(joystick);
    joystick = NULL;
  }
  numAxes = numButtons = numHats = numTrackballs = 0;
}

void Joystick::processEvents()
{
  if ( !isOpen() )
    return;

  SDL_JoystickUpdate();

  int i;
  for (i = 0; i < numAxes; i++) {
    Sint16 moved = SDL_JoystickGetAxis(joystick, i);
    if ( abs(moved) >= deadzones[i] ) {
      if ( (moved != axes[i]) ) {
        int deltaMoved = abs(axes[i] - moved);
        if ( deltaMoved >= sensitivities[i] )
          emit axisValueChanged(i, moved);
        axes[i] = moved;
        axisRepeatTimers[i].restart();
      }
      else if (autoRepeat && moved != 0) {
        if ( axisRepeatTimers[i].elapsed() >= autoRepeatDelay ) {
          emit axisValueChanged(i, moved);
          axes[i] = moved;
        }
      }
      else
        axisRepeatTimers[i].restart();
    }
    else {
      emit axisValueChanged(i, 0);
    }
  }
  for (i = 0; i < numButtons; i++) {
    Uint8 changed = SDL_JoystickGetButton(joystick, i);
    if ( (changed != buttons[i]) ) {
      emit buttonValueChanged(i, (bool) changed);
      buttons[i] = changed;
      buttonRepeatTimers[i].restart();
    } else if (autoRepeat && changed != 0) {
      if ( buttonRepeatTimers[i].elapsed() >= autoRepeatDelay ) {
        emit buttonValueChanged(i, (bool) changed);
        buttons[i] = changed;
      }
    } else
      buttonRepeatTimers[i].restart();
  }
  for (i = 0; i < numHats; i++) {
    Uint8 changed = SDL_JoystickGetHat(joystick, i);
    if ( (changed != hats[i]) ) {
      emit hatValueChanged(i, changed);
      hats[i] = changed;
      hatRepeatTimers[i].restart();
    } else if (autoRepeat && changed != 0) {
      if ( hatRepeatTimers[i].elapsed() >= autoRepeatDelay ) {
        emit hatValueChanged(i, changed);
        hats[i] = changed;
      }
    } else
      hatRepeatTimers[i].restart();
  }

  for (i = 0; i < numTrackballs; i++) {
    int dx, dy;
    SDL_JoystickGetBall(joystick, i, &dx, &dy);
    if ( dx != 0 || dy != 0 )
      emit trackballValueChanged(i, dx, dy);
  }
}

int Joystick::getAxisValue(int axis)
{
  if ( isOpen() ) {
    SDL_JoystickUpdate();
    return SDL_JoystickGetAxis(joystick, axis);
  } else
    return 0;
}
