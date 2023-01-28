import { Injectable } from '@angular/core';

@Injectable()
export class UtilsService {
  getMinWidthByScale(scale: number) {
    switch (scale) {
      case 6: return 800;
      case 8: return 1100;
      case 10: return 1400;
      case 12: return 1700;
      default: return 1100;
    }
  }
}
