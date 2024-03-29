#define LPF_MAX 45

int32_t x1_f, x2_f, y1_f, y2_f;

uint8_t filter_lpf(int32_t x, int32_t f_, uint8_t q) {
  int32_t y;
  uint8_t f = f_;
  if (f_ < 0) {
    f = 0;
  }

  if (f > LPF_MAX) f = LPF_MAX;
  if (f == 0) {
    y = 3984 * x + 7968 * x1_f + 3984 * x2_f - -2009486 * y1_f - 976845 * y2_f;
  } else if (f == 1) {
    y = 4462 * x + 8924 * x1_f + 4462 * x2_f - -2003486 * y1_f - 972758 * y2_f;
  } else if (f == 2) {
    y = 4997 * x + 9993 * x1_f + 4997 * x2_f - -1997039 * y1_f - 968450 * y2_f;
  } else if (f == 3) {
    y = 5595 * x + 11190 * x1_f + 5595 * x2_f - -1990107 * y1_f - 963911 * y2_f;
  } else if (f == 4) {
    y = 6264 * x + 12527 * x1_f + 6264 * x2_f - -1982651 * y1_f - 959130 * y2_f;
  } else if (f == 5) {
    y = 7011 * x + 14023 * x1_f + 7011 * x2_f - -1974628 * y1_f - 954097 * y2_f;
  } else if (f == 6) {
    y = 7847 * x + 15694 * x1_f + 7847 * x2_f - -1965989 * y1_f - 948801 * y2_f;
  } else if (f == 7) {
    y = 8780 * x + 17561 * x1_f + 8780 * x2_f - -1956685 * y1_f - 943231 * y2_f;
  } else if (f == 8) {
    y = 9823 * x + 19646 * x1_f + 9823 * x2_f - -1946658 * y1_f - 937375 * y2_f;
  } else if (f == 9) {
    y = 10988 * x + 21975 * x1_f + 10988 * x2_f - -1935849 * y1_f -
        931223 * y2_f;
  } else if (f == 10) {
    y = 12287 * x + 24575 * x1_f + 12287 * x2_f - -1924191 * y1_f -
        924764 * y2_f;
  } else if (f == 11) {
    y = 13738 * x + 27475 * x1_f + 13738 * x2_f - -1911611 * y1_f -
        917986 * y2_f;
  } else if (f == 12) {
    y = 15355 * x + 30711 * x1_f + 15355 * x2_f - -1898033 * y1_f -
        910879 * y2_f;
  } else if (f == 13) {
    y = 17159 * x + 34319 * x1_f + 17159 * x2_f - -1883372 * y1_f -
        903433 * y2_f;
  } else if (f == 14) {
    y = 19170 * x + 38340 * x1_f + 19170 * x2_f - -1867535 * y1_f -
        895639 * y2_f;
  } else if (f == 15) {
    y = 21410 * x + 42820 * x1_f + 21410 * x2_f - -1850423 * y1_f -
        887487 * y2_f;
  } else if (f == 16) {
    y = 23904 * x + 47809 * x1_f + 23904 * x2_f - -1831928 * y1_f -
        878970 * y2_f;
  } else if (f == 17) {
    y = 26681 * x + 53361 * x1_f + 26681 * x2_f - -1811934 * y1_f -
        870080 * y2_f;
  } else if (f == 18) {
    y = 29769 * x + 59538 * x1_f + 29769 * x2_f - -1790313 * y1_f -
        860814 * y2_f;
  } else if (f == 19) {
    y = 33203 * x + 66406 * x1_f + 33203 * x2_f - -1766931 * y1_f -
        851167 * y2_f;
  } else if (f == 20) {
    y = 37019 * x + 74038 * x1_f + 37019 * x2_f - -1741639 * y1_f -
        841139 * y2_f;
  } else if (f == 21) {
    y = 41256 * x + 82513 * x1_f + 41256 * x2_f - -1714281 * y1_f -
        830730 * y2_f;
  } else if (f == 22) {
    y = 45959 * x + 91918 * x1_f + 45959 * x2_f - -1684685 * y1_f -
        819945 * y2_f;
  } else if (f == 23) {
    y = 51174 * x + 102349 * x1_f + 51174 * x2_f - -1652670 * y1_f -
        808791 * y2_f;
  } else if (f == 24) {
    y = 56954 * x + 113908 * x1_f + 56954 * x2_f - -1618040 * y1_f -
        797280 * y2_f;
  } else if (f == 25) {
    y = 63354 * x + 126709 * x1_f + 63354 * x2_f - -1580588 * y1_f -
        785429 * y2_f;
  } else if (f == 26) {
    y = 70436 * x + 140872 * x1_f + 70436 * x2_f - -1540091 * y1_f -
        773259 * y2_f;
  } else if (f == 27) {
    y = 78266 * x + 156531 * x1_f + 78266 * x2_f - -1496312 * y1_f -
        760798 * y2_f;
  } else if (f == 28) {
    y = 86914 * x + 173828 * x1_f + 86914 * x2_f - -1449002 * y1_f -
        748081 * y2_f;
  } else if (f == 29) {
    y = 96458 * x + 192916 * x1_f + 96458 * x2_f - -1397895 * y1_f -
        735151 * y2_f;
  } else if (f == 30) {
    y = 106980 * x + 213960 * x1_f + 106980 * x2_f - -1342714 * y1_f -
        722059 * y2_f;
  } else if (f == 31) {
    y = 118569 * x + 237139 * x1_f + 118569 * x2_f - -1283165 * y1_f -
        708867 * y2_f;
  } else if (f == 32) {
    y = 131320 * x + 262641 * x1_f + 131320 * x2_f - -1218944 * y1_f -
        695649 * y2_f;
  } else if (f == 33) {
    y = 145334 * x + 290668 * x1_f + 145334 * x2_f - -1149731 * y1_f -
        682492 * y2_f;
  } else if (f == 34) {
    y = 160719 * x + 321437 * x1_f + 160719 * x2_f - -1075198 * y1_f -
        669497 * y2_f;
  } else if (f == 35) {
    y = 177588 * x + 355176 * x1_f + 177588 * x2_f - -995006 * y1_f -
        656782 * y2_f;
  } else if (f == 36) {
    y = 196063 * x + 392126 * x1_f + 196063 * x2_f - -908808 * y1_f -
        644483 * y2_f;
  } else if (f == 37) {
    y = 216271 * x + 432542 * x1_f + 216271 * x2_f - -816252 * y1_f -
        632761 * y2_f;
  } else if (f == 38) {
    y = 238347 * x + 476693 * x1_f + 238347 * x2_f - -716986 * y1_f -
        621796 * y2_f;
  } else if (f == 39) {
    y = 262429 * x + 524859 * x1_f + 262429 * x2_f - -610659 * y1_f -
        611800 * y2_f;
  } else if (f == 40) {
    y = 288665 * x + 577330 * x1_f + 288665 * x2_f - -496930 * y1_f -
        603015 * y2_f;
  } else if (f == 41) {
    y = 317205 * x + 634409 * x1_f + 317205 * x2_f - -375476 * y1_f -
        595719 * y2_f;
  } else if (f == 42) {
    y = 348202 * x + 696405 * x1_f + 348202 * x2_f - -246000 * y1_f -
        590233 * y2_f;
  } else if (f == 43) {
    y = 381815 * x + 763630 * x1_f + 381815 * x2_f - -108247 * y1_f -
        586930 * y2_f;
  } else if (f == 44) {
    y = 418197 * x + 836395 * x1_f + 418197 * x2_f - 37977 * y1_f -
        586236 * y2_f;
  } else {
    y = 457500 * x + 915000 * x1_f + 457500 * x2_f - 192774 * y1_f -
        588650 * y2_f;
  }
  y = y >> 20;

  x2_f = x1_f;
  x1_f = x;
  y2_f = y1_f;
  y1_f = y;
  return (uint8_t)(y);
}
#define HPF_MAX 49
int32_t xh1_f, xh2_f, yh1_f, yh2_f;
uint8_t filter_hpf(int32_t x, uint8_t f, uint8_t q) {
  int32_t y;
  if (f > HPF_MAX) f = HPF_MAX;
  if (f == 0) {
    y = 937604 * x + -1875209 * xh1_f + 937604 * xh2_f - -1863434 * yh1_f -
        838407 * yh2_f;
  } else if (f == 1) {
    y = 931385 * x + -1862771 * xh1_f + 931385 * xh2_f - -1849635 * yh1_f -
        827330 * yh2_f;
  } else if (f == 2) {
    y = 924841 * x + -1849681 * xh1_f + 924841 * xh2_f - -1835033 * yh1_f -
        815754 * yh2_f;
  } else if (f == 3) {
    y = 917956 * x + -1835912 * xh1_f + 917956 * xh2_f - -1819582 * yh1_f -
        803667 * yh2_f;
  } else if (f == 4) {
    y = 910717 * x + -1821434 * xh1_f + 910717 * xh2_f - -1803235 * yh1_f -
        791056 * yh2_f;
  } else if (f == 5) {
    y = 903108 * x + -1806215 * xh1_f + 903108 * xh2_f - -1785942 * yh1_f -
        777913 * yh2_f;
  } else if (f == 6) {
    y = 895114 * x + -1790227 * xh1_f + 895114 * xh2_f - -1767652 * yh1_f -
        764227 * yh2_f;
  } else if (f == 7) {
    y = 886719 * x + -1773438 * xh1_f + 886719 * xh2_f - -1748310 * yh1_f -
        749990 * yh2_f;
  } else if (f == 8) {
    y = 877908 * x + -1755817 * xh1_f + 877908 * xh2_f - -1727859 * yh1_f -
        735198 * yh2_f;
  } else if (f == 9) {
    y = 868666 * x + -1737331 * xh1_f + 868666 * xh2_f - -1706240 * yh1_f -
        719846 * yh2_f;
  } else if (f == 10) {
    y = 858975 * x + -1717950 * xh1_f + 858975 * xh2_f - -1683390 * yh1_f -
        703933 * yh2_f;
  } else if (f == 11) {
    y = 848820 * x + -1697641 * xh1_f + 848820 * xh2_f - -1659245 * yh1_f -
        687461 * yh2_f;
  } else if (f == 12) {
    y = 838186 * x + -1676373 * xh1_f + 838186 * xh2_f - -1633736 * yh1_f -
        670433 * yh2_f;
  } else if (f == 13) {
    y = 827057 * x + -1654114 * xh1_f + 827057 * xh2_f - -1606794 * yh1_f -
        652858 * yh2_f;
  } else if (f == 14) {
    y = 815417 * x + -1630834 * xh1_f + 815417 * xh2_f - -1578345 * yh1_f -
        634747 * yh2_f;
  } else if (f == 15) {
    y = 803251 * x + -1606503 * xh1_f + 803251 * xh2_f - -1548313 * yh1_f -
        616117 * yh2_f;
  } else if (f == 16) {
    y = 790545 * x + -1581090 * xh1_f + 790545 * xh2_f - -1516618 * yh1_f -
        596986 * yh2_f;
  } else if (f == 17) {
    y = 777284 * x + -1554567 * xh1_f + 777284 * xh2_f - -1483178 * yh1_f -
        577380 * yh2_f;
  } else if (f == 18) {
    y = 763454 * x + -1526908 * xh1_f + 763454 * xh2_f - -1447909 * yh1_f -
        557331 * yh2_f;
  } else if (f == 19) {
    y = 749043 * x + -1498085 * xh1_f + 749043 * xh2_f - -1410721 * yh1_f -
        536874 * yh2_f;
  } else if (f == 20) {
    y = 734038 * x + -1468075 * xh1_f + 734038 * xh2_f - -1371522 * yh1_f -
        516053 * yh2_f;
  } else if (f == 21) {
    y = 718428 * x + -1436856 * xh1_f + 718428 * xh2_f - -1330218 * yh1_f -
        494917 * yh2_f;
  } else if (f == 22) {
    y = 702202 * x + -1404405 * xh1_f + 702202 * xh2_f - -1286709 * yh1_f -
        473524 * yh2_f;
  } else if (f == 23) {
    y = 685352 * x + -1370705 * xh1_f + 685352 * xh2_f - -1240893 * yh1_f -
        451940 * yh2_f;
  } else if (f == 24) {
    y = 667869 * x + -1335738 * xh1_f + 667869 * xh2_f - -1192662 * yh1_f -
        430239 * yh2_f;
  } else if (f == 25) {
    y = 649746 * x + -1299492 * xh1_f + 649746 * xh2_f - -1141904 * yh1_f -
        408504 * yh2_f;
  } else if (f == 26) {
    y = 630977 * x + -1261953 * xh1_f + 630977 * xh2_f - -1088501 * yh1_f -
        386829 * yh2_f;
  } else if (f == 27) {
    y = 611557 * x + -1223113 * xh1_f + 611557 * xh2_f - -1032331 * yh1_f -
        365320 * yh2_f;
  } else if (f == 28) {
    y = 591483 * x + -1182965 * xh1_f + 591483 * xh2_f - -973261 * yh1_f -
        344093 * yh2_f;
  } else if (f == 29) {
    y = 570752 * x + -1141504 * xh1_f + 570752 * xh2_f - -911152 * yh1_f -
        323280 * yh2_f;
  } else if (f == 30) {
    y = 549364 * x + -1098728 * xh1_f + 549364 * xh2_f - -845855 * yh1_f -
        303025 * yh2_f;
  } else if (f == 31) {
    y = 527319 * x + -1054637 * xh1_f + 527319 * xh2_f - -777207 * yh1_f -
        283492 * yh2_f;
  } else if (f == 32) {
    y = 504618 * x + -1009235 * xh1_f + 504618 * xh2_f - -705033 * yh1_f -
        264861 * yh2_f;
  } else if (f == 33) {
    y = 481263 * x + -962526 * xh1_f + 481263 * xh2_f - -629138 * yh1_f -
        247338 * yh2_f;
  } else if (f == 34) {
    y = 457258 * x + -914517 * xh1_f + 457258 * xh2_f - -549306 * yh1_f -
        231151 * yh2_f;
  } else if (f == 35) {
    y = 432609 * x + -865218 * xh1_f + 432609 * xh2_f - -465297 * yh1_f -
        216563 * yh2_f;
  } else if (f == 36) {
    y = 407321 * x + -814642 * xh1_f + 407321 * xh2_f - -376836 * yh1_f -
        203872 * yh2_f;
  } else if (f == 37) {
    y = 381403 * x + -762806 * xh1_f + 381403 * xh2_f - -283612 * yh1_f -
        193424 * yh2_f;
  } else if (f == 38) {
    y = 354866 * x + -709732 * xh1_f + 354866 * xh2_f - -185264 * yh1_f -
        185623 * yh2_f;
  } else if (f == 39) {
    y = 327725 * x + -655450 * xh1_f + 327725 * xh2_f - -81376 * yh1_f -
        180947 * yh2_f;
  } else if (f == 40) {
    y = 300001 * x + -600002 * xh1_f + 300001 * xh2_f - 28539 * yh1_f -
        179968 * yh2_f;
  } else if (f == 41) {
    y = 271725 * x + -543450 * xh1_f + 271725 * xh2_f - 145056 * yh1_f -
        183380 * yh2_f;
  } else if (f == 42) {
    y = 242942 * x + -485884 * xh1_f + 242942 * xh2_f - 268851 * yh1_f -
        192043 * yh2_f;
  } else if (f == 43) {
    y = 213720 * x + -427439 * xh1_f + 213720 * xh2_f - 400729 * yh1_f -
        207031 * yh2_f;
  } else if (f == 44) {
    y = 184162 * x + -368324 * xh1_f + 184162 * xh2_f - 541645 * yh1_f -
        229717 * yh2_f;
  } else if (f == 45) {
    y = 154430 * x + -308860 * xh1_f + 154430 * xh2_f - 692735 * yh1_f -
        261879 * yh2_f;
  } else if (f == 46) {
    y = 124774 * x + -249549 * xh1_f + 124774 * xh2_f - 855346 * yh1_f -
        305867 * yh2_f;
  } else if (f == 47) {
    y = 95593 * x + -191186 * xh1_f + 95593 * xh2_f - 1031049 * yh1_f -
        364844 * yh2_f;
  } else if (f == 48) {
    y = 67522 * x + -135044 * xh1_f + 67522 * xh2_f - 1221641 * yh1_f -
        443154 * yh2_f;
  } else {
    y = 41597 * x + -83194 * xh1_f + 41597 * xh2_f - 1429070 * yh1_f -
        546881 * yh2_f;
  }
  y = y >> 20;

  xh2_f = xh1_f;
  xh1_f = x;
  yh2_f = yh1_f;
  yh1_f = y;
  return (uint8_t)(y);
}
