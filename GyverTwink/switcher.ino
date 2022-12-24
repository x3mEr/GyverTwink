void switchEff() {
  // поиск favorite эффектов
  while (true) {
    if (cfg.rndCh) curEff = random(0, ACTIVE_PALETTES * 2 + EFFECTS);
    else {
      if (++curEff >= ACTIVE_PALETTES * 2 + EFFECTS) curEff = 0;
    }
    if (effs[curEff].fav) break;
  }
  DEBUG("switch to: ");
  DEBUGLN(curEff);
}
