import { Component, HostListener, NgZone, OnDestroy, OnInit, } from '@angular/core';
import { ActivatedRoute, Router } from '@angular/router';
import { VariablesService } from '../_helpers/services/variables.service';
import { BackendService } from '../_helpers/services/backend.service';
import { TranslateService } from '@ngx-translate/core';
import { IntToMoneyPipe } from '../_helpers/pipes/int-to-money.pipe';
import { Subscription } from 'rxjs';
import { LOCKED_BALANCE_HELP_PAGE } from '../_shared/constants';
import { Store, Sync } from 'store';
import { distinctUntilChanged, filter } from 'rxjs/operators';

@Component({
  selector: 'app-wallet',
  templateUrl: './wallet.component.html',
  styleUrls: ['./wallet.component.scss'],
})
export class WalletComponent implements OnInit, OnDestroy {
  settingsButtonInterval;
  settingsButtonDisabled = true;
  copyAnimation = false;
  copyAnimationTimeout;
  balanceTooltip;
  walletLoaded = false;

  openDropdown: boolean;
  delWalletDialogVisible = false;
  exportHistoryDialogVisible = false;
  closeWalletId: number;
  walletSynchVisible = false;
  tabs = [
    {
      title: 'WALLET.TABS.HISTORY',
      icon: 'history',
      link: '/history',
    },
    {
      title: 'WALLET.TABS.SEND',
      icon: 'send',
      link: '/send',
    },
    {
      title: 'WALLET.TABS.RECEIVE',
      icon: 'receive',
      link: '/receive',
    },
    {
      title: 'WALLET.TABS.CONTRACTS',
      icon: 'contracts',
      link: '/contracts',
    },
    {
      title: 'WALLET.TABS.STAKING',
      icon: 'staking',
      link: '/staking',
      indicator: false,
    },
  ];
  aliasSubscription: Subscription;
  walletsSubscription: Subscription;

  @HostListener('document:keydown.shift', ['$event.key'])
  onKeyPresed() {
    if (!this.openDropdown) {
      this.walletSynchVisible = true;
    }
  }

  @HostListener('document:keyup.shift', ['$event.key'])
  onKeyUpPresed() {
    if (!this.openDropdown) {
      this.walletSynchVisible = false;
    }
  }

  @HostListener('document:click', ['$event.target'])
  public onClick(targetElement) {
    if (targetElement.dataset.target !== 'wallet-dropdown-button' && this.openDropdown) {
      this.openDropdown = false;
      this.walletSynchVisible = false;
    }
  }

  constructor(
    private route: ActivatedRoute,
    private router: Router,
    private backend: BackendService,
    public variablesService: VariablesService,
    private ngZone: NgZone,
    private translate: TranslateService,
    private intToMoneyPipe: IntToMoneyPipe,
    private store: Store
  ) {
    if (!this.variablesService.currentWallet && this.variablesService.wallets.length > 0) {
      this.variablesService.setCurrentWallet(0);
    }
    this.walletLoaded = this.variablesService.currentWallet.loaded ? true : false;
  }

  ngOnInit() {
    this.settingsButtonInterval = setInterval(() => {
      if (this.variablesService.daemon_state == 2 || this.walletLoaded) {
        this.settingsButtonDisabled = false;
        clearInterval(this.settingsButtonInterval);
      }
    }, 1000);
    this.walletsSubscription = this.store
      .select('sync')
      .pipe(filter(Boolean), distinctUntilChanged())
      .subscribe((value: any) => {
        const data = value.filter(
          (item: Sync) => item.wallet_id === this.variablesService.currentWallet.wallet_id
        )[0];
        if (data && !data.sync) {
          let in_progress;
          const values = this.store.value.sync;
          if (values && values.length) {
            in_progress = values.filter((item) => item.sync);
            this.variablesService.sync_started = !!(
              in_progress && in_progress.length
            );
            if (!in_progress) {
              this.variablesService.sync_started = false;
            }
          } else {
            this.variablesService.sync_started = false;
          }
        }
      });

    this.copyAnimation = false;
    if (this.variablesService.currentWallet.alias.hasOwnProperty('name')) {
      this.variablesService.currentWallet.wakeAlias = false;
    }
    this.aliasSubscription = this.variablesService.getAliasChangedEvent.subscribe(
      () => {
        if (this.variablesService.currentWallet.alias.hasOwnProperty('name')) {
          this.variablesService.currentWallet.wakeAlias = false;
        }
      }
    );
    this.updateWalletStatus();
  }

  copyAddress() {
    this.backend.setClipboard(this.variablesService.currentWallet.address);
    this.copyAnimation = true;
    this.copyAnimationTimeout = window.setTimeout(() => {
      this.copyAnimation = false;
    }, 2000);
  }

  getTooltip() {
    this.balanceTooltip = document.createElement('div');
    const available = document.createElement('span');
    available.setAttribute('class', 'available');
    available.innerHTML = this.translate.instant('WALLET.AVAILABLE_BALANCE', {
      available: this.intToMoneyPipe.transform(
        this.variablesService.currentWallet.unlocked_balance
      ),
      currency: this.variablesService.defaultCurrency,
    });
    this.balanceTooltip.appendChild(available);
    const locked = document.createElement('span');
    locked.setAttribute('class', 'locked');
    locked.innerHTML = this.translate.instant('WALLET.LOCKED_BALANCE', {
      locked: this.intToMoneyPipe.transform(
        this.variablesService.currentWallet.balance.minus(
          this.variablesService.currentWallet.unlocked_balance
        )
      ),
      currency: this.variablesService.defaultCurrency,
    });
    this.balanceTooltip.appendChild(locked);
    const link = document.createElement('span');
    link.setAttribute('class', 'link');
    link.innerHTML = this.translate.instant('WALLET.LOCKED_BALANCE_LINK');
    link.addEventListener('click', () => {
      this.openInBrowser(LOCKED_BALANCE_HELP_PAGE);
    });
    this.balanceTooltip.appendChild(link);
    return this.balanceTooltip;
  }

  onHideTooltip() {
    this.balanceTooltip = null;
  }

  openInBrowser(link) {
    this.backend.openUrlInBrowser(link);
  }

  togleMenuDropdown() {
    if (!this.openDropdown) {
      this.openDropdown = true;
    } else {
      this.openDropdown = false;
      this.walletSynchVisible = false;
    }
  }

  resyncCurrentWallet(id) {
    this.backend.resyncWallet(id);
  }

  showConfirmDialog(wallet_id) {
    this.delWalletDialogVisible = true;
    this.closeWalletId = wallet_id;
  }

  closeExportModal(confirmed: boolean) {
    if (confirmed) {
      this.exportHistoryDialogVisible = false;
    }
  }

  confirmed(confirmed: boolean) {
    if (confirmed) {
      this.closeWallet(this.closeWalletId);
    }
    this.delWalletDialogVisible = false;
  }

  closeWallet(wallet_id) {
    this.backend.closeWallet(wallet_id, () => {
      for (let i = this.variablesService.wallets.length - 1; i >= 0; i--) {
        if (this.variablesService.wallets[i].wallet_id === this.variablesService.currentWallet.wallet_id) {
          this.variablesService.wallets.splice(i, 1);
        }
      }
      this.ngZone.run(() => {
        if (this.variablesService.wallets.length) {
          this.variablesService.currentWallet = this.variablesService.wallets[0];
          this.router.navigate(['/wallet/']);
        } else {
          this.router.navigate(['/']);
        }
      });
      if (this.variablesService.appPass) {
        this.backend.storeSecureAppData();
      }
    });
  }

  ngOnDestroy() {
    this.aliasSubscription.unsubscribe();
    if (this.walletsSubscription) {
      this.walletsSubscription.unsubscribe();
    }
    clearTimeout(this.copyAnimationTimeout);
  }


  updateWalletStatus() {
    this.backend.eventSubscribe('wallet_sync_progress', (data) => {
      const wallet_id = data.wallet_id;
      if (wallet_id === this.variablesService.currentWallet.wallet_id) {
        this.ngZone.run(() => {
          this.walletLoaded = false;
        });
      }
    });
    this.backend.eventSubscribe('update_wallet_status', (data) => {
      const wallet_state = data.wallet_state;
      const wallet_id = data.wallet_id;
      this.ngZone.run(() => {
        if (wallet_state === 2 && wallet_id === this.variablesService.currentWallet.wallet_id) {
          this.walletLoaded =
            (this.variablesService.getWallet(this.variablesService.currentWallet.wallet_id) !== null &&
              this.variablesService.getWallet(this.variablesService.currentWallet.wallet_id).loaded)
              ? true
              : false;
        } else {
          this.walletLoaded = false;
        }
      });
    });
  }
}
