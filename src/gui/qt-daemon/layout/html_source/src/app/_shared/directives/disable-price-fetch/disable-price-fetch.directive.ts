import { Directive, OnDestroy, TemplateRef, ViewContainerRef } from '@angular/core';
import { VariablesService } from '../../../_helpers/services/variables.service';
import { Subject } from 'rxjs';
import { takeUntil } from 'rxjs/operators';

@Directive({
  selector: '[appDisablePriceFetch]'
})
export class DisablePriceFetchDirective implements OnDestroy {

  private _destroy$: Subject<never> = new Subject<never>();

  constructor(
    private _variablesService: VariablesService,
    private _templateRef: TemplateRef<any>,
    private _viewContainer: ViewContainerRef
  ) {
    this._variablesService.disable_price_fetch$.pipe(takeUntil(this._destroy$)).subscribe((disable_price_fetch: boolean) => {
      return !disable_price_fetch ? this._viewContainer.createEmbeddedView(this._templateRef) : this._viewContainer.clear();
    });
  }

  ngOnDestroy() {
    this._destroy$.next();
  }

}
