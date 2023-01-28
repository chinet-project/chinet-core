import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { DisablePriceFetchDirective } from './disable-price-fetch.directive';

@NgModule({
  declarations: [DisablePriceFetchDirective],
  imports: [
    CommonModule
  ],
  exports: [DisablePriceFetchDirective]
})
export class DisablePriceFetchModule { }
