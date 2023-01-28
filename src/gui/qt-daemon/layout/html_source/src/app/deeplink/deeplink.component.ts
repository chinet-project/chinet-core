import { takeUntil } from 'rxjs/operators';
import { Subject } from 'rxjs';
import { DeeplinkParams, PushOffer, Wallet } from './../_helpers/models/wallet.model';
import { Component, OnInit, OnDestroy, NgZone } from '@angular/core';
import { Router } from '@angular/router';
import { VariablesService } from '../_helpers/services/variables.service';
import { BackendService } from '../_helpers/services/backend.service';
import { MIXIN } from '../_shared/constants';
import { BigNumber } from 'bignumber.js';

@Component({
  selector: 'app-deeplink',
  templateUrl: './deeplink.component.html',
  styleUrls: ['./deeplink.component.scss']
})
export class DeeplinkComponent implements OnInit, OnDestroy {
  deeplink: string | null = null;
  secondStep = false;
  walletToPayId = 0;
  nextStepInterval
  marketplaceModalShow = true;
  copyAnimation = false;
  marketplaceConfirmHash: any = null
  sendRoute = false;
  actionData: DeeplinkParams = {}
  defaultMixin = MIXIN
  walletsTopay: Array<Wallet> = [];
  private destroy$ = new Subject<never>();
  constructor(
    private _router: Router,
    public variablesService: VariablesService,
    private backend: BackendService,
    private ngZone: NgZone,
  ) {
    this.variablesService.deeplink$.pipe(takeUntil(this.destroy$)).subscribe((data) => {
      this.ngZone.run(() => {
        if (data) {
          this.deeplink = data;
          this.actionData = {};
          this.walletsTopay = this.variablesService.wallets.filter(wallet => !wallet.is_watch_only || !wallet.is_auditable)
          if (this.walletsTopay.length === 0) {
            this.canselAction()
            return
          }
          this.actionData = this.parceString(this.deeplink);
          if (this.walletsTopay.length === 1) {
            if (variablesService.daemon_state === 2 && variablesService.sync_started === false) {
              this.walletToPayId = this.walletsTopay[0].wallet_id
              this.nextStep()
            } else {
              this.nextStepInterval = setInterval(() => {
                if (variablesService.daemon_state === 2 && variablesService.sync_started === false) {
                  this.walletToPayId = this.walletsTopay[0].wallet_id
                  this.nextStep()
                  clearInterval(this.nextStepInterval)
                }
              }, 1500)
            }
          }
        } else {
          this.deeplink = null;
        }
      })
    });
  }

  ngOnInit() {
  }

  parceString(string) {
    const qoutesRex = new RegExp(/'|"|”|%E2%80%9D|%22/g);
    const spaceSymbolRex = new RegExp(/%20/g);
    const newobj = {};

    const newstring = string.substr(5) // delete chinet:;
    newstring.split('&').forEach((string) => {
      const [key, value] = string.split('=');
      newobj[key] = value.replace(qoutesRex, '').replace(spaceSymbolRex, " ").trim();
    });
    return newobj
  }

  canselAction() {
    this.deeplink = null
    this.variablesService.deeplink$.next(null)
    this.variablesService.sendActionData$.next({});
    this.actionData = {};
    this.secondStep = false;
  }

  marketplaceSend() {
    let offerObject: PushOffer = {
      wallet_id: this.walletToPayId,
      od: {
        ap: this.actionData.price || '',
        at: '1',
        cat: this.actionData.category || '',
        cnt: this.actionData.contact || '',
        com: this.actionData.comment || this.actionData.comments || '',
        do: this.actionData.description || '',
        et: 10,
        fee: new BigNumber('' + ((+this.actionData.fee || +this.variablesService.default_fee) * 1000000000000)),
        lci: '',
        lco: 'World Wide',
        ot: 1,
        pt: 'Credit cards, BTC, CHN, ETH',
        t: this.actionData.title || '',
        url: this.actionData.url || this.actionData.img_url || '',
      },
    }
    this.backend.push_offer(offerObject, (Status, data) => {
      if (data.success) {
        this.marketplaceModalShow = false;
        this.marketplaceConfirmHash = data.tx_hash
      } else {
        this.canselAction()
      }
    })
  }


  copyHash() {
    this.backend.setClipboard(this.marketplaceConfirmHash);
    this.copyAnimation = true;
    setTimeout(() => {
      this.copyAnimation = false;
    }, 2000);
  }

  nextStep() {
    if (this.actionData.action === "send") {
      this.ngZone.run(() => {
        this.variablesService.sendActionData$.next(this.actionData)
        this.variablesService.deeplink$.next(null)
        this.variablesService.setCurrentWallet(this.walletToPayId)
        this._router.navigate(['/wallet/send'])
        this.secondStep = false
      })
    } else if (this.actionData.action === "escrow") {
      this.ngZone.run(() => {
        this.variablesService.sendActionData$.next(this.actionData)
        this.variablesService.deeplink$.next(null)
        this.variablesService.setCurrentWallet(this.walletToPayId)
        this._router.navigate(['/wallet/contracts/purchase'])
        this.secondStep = false
      })
    } else {
      this.secondStep = true
    }
  }

  ngOnDestroy() {
    this.destroy$.next();
    this.variablesService.deeplink$.next(null);
  }
}
