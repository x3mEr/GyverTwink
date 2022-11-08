void receive(byte[] ubuf) {
  if (ubuf[0] != 'G' || ubuf[1] != 'T') return;
  int[] data = new int[11];
  for (int i = 0; i < min(ubuf.length - 2,11); i++) {
    data[i] = int(ubuf[i+2]);
  }
  
  switch (data[0]) {
    case 6:
      println("receive " + data[0] + " " + data[1] + " " + data[2] + " s:"+ubuf.length);
      for(int i=data[1];i<(data[1] + data[2]);i++) {
        xy[i][0] = ubuf[5+(i%50)*2];
        xy[i][1] = ubuf[5+(i%50)*2+1];
      }
    
    break;
  }
  

  if (parseMode != data[0]) return;

  switch (data[0]) {
  case 0: // Поиск
    String ip = brIP.substring(0, brIP.lastIndexOf('.')+1) + str(data[1]);
    if (!ips.hasValue(ip)) ips.append(ip);
    break;

  case 1: // Настройки 
    searchF = false;
    leds.text = str(data[1]);
    strips.text = str(data[2]);
    ledsCount = data[1] * data[2];
    power.value = boolean(data[3]);
    bri.value = data[4];
    auto.value = boolean(data[5]);
    rnd.value = boolean(data[6]);
    prd.value = data[7];
    offT.value = boolean(data[8]);
    offS.value = data[9];
    break;

  case 4: // Эффект
    fav.value = boolean(data[1]);
    scl.value = data[2];
    spd.value = data[3];
    break;
  case 5: // Размер окна рисования
    XX = data[1];
    YY = data[2];
    break;
  }
}
