/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <iostream>
#include <stdio.h> // TODO BSS remove
#include "xmlinterface.h"
#include "radio.hxx"

using namespace std;

XmlInterface::XmlInterface(QTextStream &stream):
  stream(stream)
{
}

bool XmlInterface::load(RadioData &radioData)
{
  try
  {
   // auto_ptr<radio> r ( radio());

    //r->global_settings().ownerName("Bertrand");

/*    for (hello_t::name_const_iterator i (h->name ().begin ());
         i != h->name ().end ();
         ++i)
    {
      cerr << h->greeting () << ", " << *i << "!" << endl;
    }
*/
    return true;
  }
  catch (const xml_schema::exception& e)
  {
    cerr << e << endl;
    return false;
  }
}

bool XmlInterface::loadxml(RadioData &radioData, QDomDocument &doc)
{
  return false;
}

void saveRadioSettings(GeneralSettings & settings, global_settings & gs)
{
  Calibration rudderCalib(settings.calibMid[0], settings.calibSpanNeg[0], settings.calibSpanPos[0]);
  Calibration throttleCalib(settings.calibMid[1], settings.calibSpanNeg[1], settings.calibSpanPos[1]);
  Calibration aileronCalib(settings.calibMid[2], settings.calibSpanNeg[2], settings.calibSpanPos[2]);
  Calibration elevatorCalib(settings.calibMid[3], settings.calibSpanNeg[3], settings.calibSpanPos[3]);
  Calibration p1Calib(settings.calibMid[4], settings.calibSpanNeg[4], settings.calibSpanPos[4]);
  Calibration p2Calib(settings.calibMid[5], settings.calibSpanNeg[5], settings.calibSpanPos[5]);
  Calibration p3Calib(settings.calibMid[6], settings.calibSpanNeg[6], settings.calibSpanPos[6]);
  gs.calibration(calibration(rudderCalib, throttleCalib, aileronCalib, elevatorCalib, p1Calib, p2Calib, p3Calib));

  // TODO BSS settings.currModelIndex;
  gs.contrast(settings.contrast);
  gs.battery(battery(settings.txVoltageCalibration, settings.vBatWarn));

  gs.ppm_input_calibration(ppm_input_calibration(settings.PPM_Multiplier,
                                                 PPMCalibration(settings.trainer.calib[0]),
                                                 PPMCalibration(settings.trainer.calib[1]),
                                                 PPMCalibration(settings.trainer.calib[2]),
                                                 PPMCalibration(settings.trainer.calib[3])));
  gs.view(settings.view);
  warnings warns;
  warnings::warning_sequence & ws (warns.warning());
  if (settings.disableThrottleWarning)
    ws.push_back(warning("throttle", false));
/* TODO    if (settings.disableSwitchWarning)
    ws.push_back(warning("switchs", false)); */
  if (settings.disableMemoryWarning)
    ws.push_back(warning("memory", false));
  if (settings.disableAlarmWarning)
    ws.push_back(warning("silent", false));
  if (!ws.empty())
    gs.warnings(warns);
  gs.beeper(settings.beeperMode);
  sticks stks(settings.stickMode);
  gs.sticks(stks);
  gs.inactivity_timer(settings.inactivityTimer);
  if (settings.minuteBeep || settings.preBeep || settings.flashBeep) {
    timer_beeps tb;
    if (settings.minuteBeep)
      tb.minute(true);
    if (settings.preBeep)
      tb.countdown(true);
    if (settings.flashBeep)
      tb.screen_flash(true);
    gs.timer_beeps(tb);
  }
  /* TODO if (settings.disableSplashScreen)
    gs.splash_screen(false); */
  /* TODO if (settings.backlightDelay)
    gs.light_auto_off_timer(settings.lightAutoOff); */
  gs.default_channel_order(settings.templateSetup);
  if (strlen(settings.ownerName))
    gs.owner_name(settings.ownerName);
}

void saveModel(ModelData & m, model & xm)
{
  /* TODO
  model::timer_sequence & timer_sequence (xm.timer());
  for (int i=0; i<2; i++) {
     TimerData & t = m.timers[i];
     if (t.mode) {
      timer xt(TimerSwitch::value(t.mode+TMR_NUM_OPTION), direction::value(t.dir), timer::value_type(t.val/3600, (t.val%3600)/60, t.val%60));
      timer_sequence.push_back(xt);
    }
  }
  */
  if (m.moduleData[0].protocol != PULSES_PPM || m.moduleData[0].channelsCount != 8 || m.moduleData[0].ppm.delay != 300 || m.moduleData[0].ppm.pulsePol != 0) {
    modulation mod;
    mod.type(modulation::type_type::value(m.moduleData[0].protocol));
    mod.channels(m.moduleData[0].channelsCount);
    mod.delay(m.moduleData[0].ppm.delay);
    mod.polarity(modulation::polarity_type::value(m.moduleData[0].ppm.pulsePol));
    xm.modulation(mod);
  }
  {
    trims xtrims;
    xtrims.extended(m.extendedTrims);
    xtrims.throttleTrim(m.thrTrim);
    xm.trims(xtrims);
  }
  if (m.beepANACenter) {
    centerBeep xcb;
    if (m.beepANACenter & (1 << 0)) xcb.rudder(true);
    if (m.beepANACenter & (1 << 1)) xcb.elevator(true);
    if (m.beepANACenter & (1 << 2)) xcb.throttle(true);
    if (m.beepANACenter & (1 << 3)) xcb.aileron(true);
    if (m.beepANACenter & (1 << 4)) xcb.p1(true);
    if (m.beepANACenter & (1 << 5)) xcb.p2(true);
    if (m.beepANACenter & (1 << 6)) xcb.p3(true);
    xm.centerBeep(xcb);
  }

  phases xphases;
  phases::phase_sequence & phases_sequence (xphases.phase());
  for (int i=0; i<CPN_MAX_FLIGHT_MODES; i++) {
    FlightModeData & p = m.flightModeData[i];
    if (i == 0 || p.swtch.type!=SWITCH_TYPE_NONE) {
      TrimType xtrim[4];
      for (int j=0; j<4; j++) {
        if (m.flightModeData[i].trimRef[j] >= 0)
          xtrim[j].phaseRef(m.flightModeData[i].trimRef[j]);
        else
          xtrim[j].value(m.flightModeData[i].trim[j]);
      }
      trims1 xtrims(xtrim[0], xtrim[1], xtrim[2], xtrim[3]);
      phase xphase(xtrims, i);
      if (p.fadeIn) xphase.fadeIn(p.fadeIn);
      if (p.fadeOut) xphase.fadeOut(p.fadeOut);
      phases_sequence.push_back(xphase);
    }
  }
  xm.phases(xphases);
}

bool XmlInterface::save(RadioData &radioData)
{
 GeneralSettings &settings = radioData.generalSettings;

 try
  {
    xml_schema::namespace_infomap map;
    map[""].name = "";
    map[""].schema = "radio.xsd";

    radio r;

    // the general settings
    global_settings gs;
    saveRadioSettings(settings, gs);
    r.global_settings(gs);

    // the models
    models xml_models;
    models::model_sequence & model_sequence (xml_models.model());
    for (int i=0; i<radioData.models.size(); i++) {
      ModelData & m = radioData.models[i];
      if (m.used) {
        model xm(m.name);
        saveModel(m, xm);
        model_sequence.push_back(xm);
      }
    }
    if (!model_sequence.empty())
      r.models(xml_models);

    std::stringstream ss;
    radio_(ss, r, map);

    stream << QString::fromStdString(ss.str());

    return true;
  }
  catch (const xml_schema::exception& e)
  {
    cerr << e << endl;
    return false;
  }
}


