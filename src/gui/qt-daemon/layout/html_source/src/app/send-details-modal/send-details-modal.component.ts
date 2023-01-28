import { ChangeDetectionStrategy, Component, ElementRef, EventEmitter, Input, OnDestroy, OnInit, Output, ViewChild } from '@angular/core';
import {
  AsyncCommandResults,
  BackendService,
  CurrentActionState,
  ResponseAsyncTransfer,
  StatusCurrentActionState
} from '../_helpers/services/backend.service';
import { BehaviorSubject, Subject } from 'rxjs';
import { VariablesService } from '../_helpers/services/variables.service';
import { filter, takeUntil } from 'rxjs/operators';

const successfulStatuses: string[] = [
  StatusCurrentActionState.STATE_SENDING,
  StatusCurrentActionState.STATE_SENT_SUCCESS,
  StatusCurrentActionState.STATE_INITIALIZING,
  StatusCurrentActionState.STATE_DOWNLOADING_CONSENSUS,
  StatusCurrentActionState.STATE_MAKING_TUNNEL_A,
  StatusCurrentActionState.STATE_MAKING_TUNNEL_B,
  StatusCurrentActionState.STATE_CREATING_STREAM,
  StatusCurrentActionState.STATE_SUCCESS
];

const failedStatuses: string[] = [StatusCurrentActionState.STATE_SEND_FAILED, StatusCurrentActionState.STATE_FAILED];

@Component({
  selector: 'app-send-details-modal',
  templateUrl: './send-details-modal.component.html',
  styleUrls: ['./send-details-modal.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush
})
export class SendDetailsModalComponent implements OnInit, OnDestroy {
  /** Working id is traceable */
  @Input() job_id: number;

  @Output() close: EventEmitter<never> = new EventEmitter<never>();

  @ViewChild('elDetailsWrapper') elDetailsWrapper: ElementRef;

  /** BehaviorSubject with ResponseAsyncTransfer */
  responseData$ = new BehaviorSubject<ResponseAsyncTransfer>(null);

  /** BehaviorSubject flag for stateDetails */
  stateDetails$ = new BehaviorSubject<boolean>(false);

  /** BehaviorSubject with CurrentActionState */
  currentActionState$ = new BehaviorSubject<CurrentActionState>(null);

  get currentActionState(): CurrentActionState {
    return this.currentActionState$.value;
  }

  /** BehaviorSubject with CurrentActionState[] */
  currentActionStates$ = new BehaviorSubject<CurrentActionState[]>([]);

  get currentActionStates(): CurrentActionState[] {
    return this.currentActionStates$.value;
  }

  /** True, if currentActionState.status = success */
  get isSentSuccess(): boolean {
    return this.currentActionState && this.currentActionState.status === StatusCurrentActionState.STATE_SENT_SUCCESS;
  }

  /** True, if currentActionState.status = failed */
  get isSentFailed(): boolean {
    return this.currentActionState && this.currentActionState.status === StatusCurrentActionState.STATE_SEND_FAILED;
  }

  /** True, responseData$ or currentActionStates$ not empty */
  get isDetailsNotEmpty(): boolean {
    return !!(this.responseData$.value || this.currentActionStates$.value.length);
  }

  private _destroy$: Subject<never> = new Subject<never>();

  constructor(
    private _backendService: BackendService,
    private _variablesService: VariablesService) {
  }

  ngOnInit() {
    const { currentWallet: { wallet_id }, settings: { appUseTor } } = this._variablesService;

    if (appUseTor) {
      /** Listening handleCurrentActionState */
      this._backendService.handleCurrentActionState$
        .pipe(takeUntil(this._destroy$))
        .subscribe((currentActionState: CurrentActionState) => {
          this.currentActionState$.next(currentActionState);
          this.currentActionStates$.next([...this.currentActionStates, currentActionState]);
        });
    } else {
      const actionState: CurrentActionState = {
        status: StatusCurrentActionState.STATE_INITIALIZING,
        wallet_id
      };
      this.currentActionState$.next(actionState);
      this.currentActionStates$.next([...this.currentActionStates, actionState]);
    }

    /** Listening dispatchAsyncCallResult */
    this._backendService.dispatchAsyncCallResult$
      .pipe(
        filter(({ job_id, response }: AsyncCommandResults) => this.job_id === job_id && !!response),
        takeUntil(this._destroy$)
      ).subscribe(({ response }: AsyncCommandResults) => {
      const { response_data: { success } } = response;
      if (!appUseTor || !success) {
        const actionState: CurrentActionState = {
          status: success ? StatusCurrentActionState.STATE_SENT_SUCCESS : StatusCurrentActionState.STATE_SEND_FAILED,
          wallet_id
        };
        this.currentActionState$.next(actionState);
        this.currentActionStates$.next([...this.currentActionStates, actionState]);
      }

      this.responseData$.next(response);
    });
  }

  ngOnDestroy() {
    this._destroy$.next();
  }

  /** Show/Hide details transaction */
  toggleDetails(): any {
    this.stateDetails$.next(!this.stateDetails$.value);
    setTimeout(() => this._scrollToBottomDetailsWrapper(), 100);
  }

  /** identification item by *ngFor */
  trackBy(index: number): number {
    return index;
  }

  /** True, if status success */
  isSuccess(action: CurrentActionState): boolean {
    return successfulStatuses.includes(action && action.status);
  }

  /** True, if status failed */
  isFailed(action: CurrentActionState): boolean {
    return failedStatuses.includes(action && action.status);
  }

  /** Scroll elDetailsWrapper to bottom */
  private _scrollToBottomDetailsWrapper() {
    if (this.elDetailsWrapper) {
      const { nativeElement } = this.elDetailsWrapper;
      nativeElement.scrollTop = nativeElement.scrollHeight;
    }
  }
}
