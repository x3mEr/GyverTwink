byte curTab = 0;
int currentX=0, currentY=0;

TextInput strips = new TextInput();
TextInput leds = new TextInput();
TextInput subnet = new TextInput();
DropDown dropIP = new DropDown();
DropDown dropEff = new DropDown();
Toggle power = new Toggle();
Toggle offT = new Toggle();
Toggle auto = new Toggle();
Toggle rnd = new Toggle();
Slider bri = new Slider();
Slider prd = new Slider();
Slider offS = new Slider();
Toggle fav = new Toggle();
Slider scl = new Slider();
Slider spd = new Slider();

String[] effs = {
  "0. Party_grad", 
  "1. Raibow_grad", 
  "2. Stripe_grad", 
  "3. Sunset_grad", 
  "4. Pepsi_grad", 
  "5. Warm_grad", 
  "6. Cold_grad", 
  "7. Hot_grad", 
  "8. Pink_grad", 
  "9. Cyber_grad", 
  "10. RedWhite_grad", 
  "11. Party_noise", 
  "12. Raibow_noise", 
  "13. Stripe_noise", 
  "14. Sunset_noise", 
  "15. Pepsi_noise", 
  "16. Warm_noise", 
  "17. Cold_noise", 
  "18. Hot_noise", 
  "19. Pink_noise", 
  "20. Cyber_noise", 
  "21. RedWhite_noise",

  "22. Sparkles",
  "23. Rainbow_Vert",
  "24. Rainbow_Horiz",
  "25. Rainbow_Diag",
  "26. Color",
  "27. Colors_change",  
  "28. Fire",
  "29. White_Fire",  
};

void ui() {
  uiFill();
  // ====== TABS =======
  int w = width / 4;
  int h = w / 2;
  int y = height - h;

  if (IconButton("wrench", 0, y, w, h, curTab == 0)) switchCfg();
  if (IconButton("adjust", w*1, y, w, h, curTab == 1)) switchEffects();
  if (IconButton("camera", w*2, y, w, h, curTab == 2)) switchCalib();
  if (IconButton("paintbrush", w*3, y, w, h, curTab == 3)) switchPaint();

  if (curTab == 0) cfgTab();
  if (curTab == 1) effTab();
  if (curTab == 2) calibTab();
  if (curTab == 3) paintTab();
}

void cfgTab() {
  uiGlobalX(offs);
  uiResetStep(20);
  LabelCenter("GyverTwink", 20);
  Divider(width-offs*2);

  Label("Subnet:", 15);
  Label("Connection:", 15);
  if (found) {
    Divider(width-offs*2);
    Label("Strips amount:", 15);
    Label("LEDs per strip:", 15);
    Label("Power:", 15);
    Label("Brightness:", 15);
    Divider(width-offs*2);
    Label("Off timer:", 15);
    Label("Turn off in [1-240m]:", 15);
    Divider(width-offs*2);
    Label("Switch effect:", 15);
    Label("Auto:", 15);
    Label("Random:", 15);
    Label("Period [1-10m]:", 15);
  }

  uiResetStep(20);
  uiStep();
  uiStep();

  if (found) { 
    uiStep();
    uiStep();
    uiStep();
    if (strips.show(WW, uiStep(), W) && androidMode) openKeyboard();
    if (strips.done()) {
      if (androidMode) closeKeyboard();
      sendData(new int[] {2, 9, int(strips.text)});
    }
    
    if (leds.show(WW, uiStep(), W) && androidMode) openKeyboard();
    if (leds.done()) {
      if (androidMode) closeKeyboard();
      sendData(new int[] {2, 0, int(leds.text)});
    }

    if (power.show(WW, uiStep())) sendData(new int[] {2, 1, int(power.value)});
    if (bri.show(0, 255, WW, uiStep(), W)) sendData(new int[] {2, 2, int(bri.value)});
    uiStep();
    if (offT.show(WW, uiStep())) sendData(new int[] {2, 7, int(offT.value)});
    if (offS.show(0, 250, WW, uiStep(), W)) sendData(new int[] {2, 8, int(offS.value)});
    uiStep();
    if (Button("Next effect", WW, uiStep(), W)) sendData(new int[] {2, 6});
    if (auto.show(WW, uiStep())) sendData(new int[] {2, 3, int(auto.value)});
    if (rnd.show(WW, uiStep())) sendData(new int[] {2, 4, int(rnd.value)});
    if (prd.show(1, 10, WW, uiStep(), W)) sendData(new int[] {2, 5, int(prd.value)});
  }

  uiResetStep(20);
  uiStep();
  uiStep();
  if (subnet.show(WW, uiStep(), W) && androidMode) openKeyboard();
  if (subnet.done()) {
    if (androidMode) closeKeyboard();
    file[0] = subnet.text;
    saveStrings("subnet.txt", file);
  }
  if (dropIP.show(ips.array(), WW, uiStep(), W-s_height)) {
    curIP = ips.get(dropIP.getSelected());
    requestCfg();
  }
  if (IconButton("sync", WW + W-s_height, uiPrevStep())) startSearch();
}

void effTab() {
  uiGlobalX(offs);
  uiResetStep(50);
  uiGlobalX(offs);
  if (found) {
    Label("Effect:", 15);
    Label("Favorite:", 15);
    Label("Scale:", 15);
    Label("Speed:", 15);

    uiResetStep(60);
    uiStep();
    if (fav.show(WW, uiStep())) sendData(new int[] {4, 1, int(fav.value)});
    if (scl.show(0, 255, WW, uiStep(), W)) sendData(new int[] {4, 2, int(scl.value)});
    if (spd.show(0, 255, WW, uiStep(), W)) sendData(new int[] {4, 3, int(spd.value)});

    uiResetStep(50);

    //if (androidMode) uiSetScale(androidScale*0.8);
    //else uiSetScale(pcScale*0.7);
    if (dropEff.show(effs, WW, uiStep(), W-s_height)) {
      sendData(new int[] {4, 0, dropEff.selected});
      parseMode = 4;
    }
    //if (androidMode) uiSetScale(androidScale);
    //else uiSetScale(pcScale);
  } else Label("No devices detected!", 15);
}

void calibTab() { 
  if (found) {
    // Камера не стартовала в PC режиме
    if (!androidMode && Wcam == null) return;

    if (camReady) {
      camReady = false;
      readCam();
      makeMap(1);
      findMax();
    }

    PImage frameScaled = frame.copy();
    frameScaled.resize(0, height*4/5);
    image(frameScaled, (width-frameScaled.width)/2, 0);
    if (calibF) {
      frameScaled = ring.copy();
      frameScaled.resize(0, height*4/5);
      image(frameScaled, (width-frameScaled.width)/2, 0);
    }
    //image(frame, (width-frame.width)/2, 0);
    //if (calibF) image(ring, (width-ring.width)/2, 0);

    uiResetStep(height - width/6 - 1*_step_y);
    uiResetX(0);
    uiGlobalX(0);

    if (Button("Start", 15)) {
      calibF = true;
      sendData(new int[] {3, 0});
      calibCount = 0;
      actionTmr = millis() + 2000;
    }

    if (Button("Stop", 165, uiPrevStep())) {
      calibF = false;
      sendData(new int[] {3, 2});
      calibCount = 0;
    }
    
    Label(str(calibCount)+" of " + str(int(strips.text) * int(leds.text)) + " (" + str(calibCount*100/((int(strips.text)) * int(leds.text)+1))+"%)", 15, uiPrevX()+15, uiPrevStep());

} else {
    uiGlobalX(offs);
    uiResetStep(50);
    uiGlobalX(offs);
    Label("No devices detected!", 15);
  }
}

void paintTab() {
  uiGlobalX(offs);
  uiResetStep(15);
  LabelCenter("Paint", 20);
  //println("0:x:" + xy[0][0] + " 0:y:" + xy[0][1]);
  Label("XX:" + XX + " " + _x_offs + " YY:" + YY + " " + _pos_y + " cX:" + currentX + " cY:" + currentY, 15);
  int step = 7;
  int size = 12;
  int xOffset = (width-(X * step)) / 2;
  int yOffset = _pos_y;
  int tMedFontSize = medFontSize;
  medFontSize = 6;
  if(xy != null) {
    for(int i=0;i<ledsCount;i++) {
      currentX = xy[i][0]; currentY = xy[i][1];
      int c = xy[i][2];
     
      if(Button(((c==1)?"O":" "), xOffset + currentX*step, yOffset +currentY*step, size, size)) {
        xy[i][2] = (byte)(1 - xy[i][2]);
        sendData(new int[] {5, 1, currentX, currentY, 2, xy[i][2]});
      }
    }
  }

  medFontSize = tMedFontSize;
  
  
  
}


void switchCfg() {
  curTab = 0;
  sendData(new int[] {5, 2});
  sendData(new int[] {2, 7});
  stopCam();
}
void switchEffects() {
  curTab = 1;
  stopCam();
  sendData(new int[] {5, 2});
  sendData(new int[] {4, 0, dropEff.selected});
  parseMode = 4;
}
void switchCalib() {
  curTab = 2;
  sendData(new int[] {5, 2});
  if (found) startCam();
}
void switchPaint() {
  curTab = 3;
  stopCam();
 
  parseMode = 5;
  xy = new byte[ledsCount][3];
  sendData(new int[] {5, 0});
}
