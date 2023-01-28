import { Component, NgZone, OnInit, OnDestroy } from '@angular/core';
import { ActivatedRoute, NavigationEnd, NavigationStart, Router } from '@angular/router';
import { VariablesService } from '../_helpers/services/variables.service';
import { BackendService } from '../_helpers/services/backend.service';
import { ModalService } from '../_helpers/services/modal.service';
import { AUDITABLE_WALLET_HELP_PAGE } from '../_shared/constants';
import { DOWNLOADS_PAGE_URL } from '../_shared/constants';
import { CdkDragDrop, moveItemInArray } from '@angular/cdk/drag-drop';
import { Wallet } from '../_helpers/models/wallet.model';


@Component({
  selector: 'app-sidebar',
  templateUrl: './sidebar.component.html',
  styleUrls: ['./sidebar.component.scss']
})
export class SidebarComponent implements OnInit, OnDestroy {
  walletSubRouting;
  walletActive: number;
  isModalDialogVisible = false;
  closeWalletId: number;

  constructor(
    private route: ActivatedRoute,
    private router: Router,
    public variablesService: VariablesService,
    private backend: BackendService,
    private modal: ModalService,
    private ngZone: NgZone
  ) {

  }

  ngOnInit() {

  }

  goMainPage() {
    if (this.route.snapshot.queryParams && this.route.snapshot.queryParams.prevUrl === 'login') {
      this.ngZone.run(() => {
        this.router.navigate(['/'], { queryParams: { prevUrl: 'login' } });
      });
    } else {
      this.ngZone.run(() => {
        this.router.navigate(['/']);
      });
    }

  };

  selectWallet(id: number) {
    this.ngZone.run(() => {
      this.variablesService.setCurrentWallet(id)
      this.router.navigate(['/wallet/history']);
    });
  }

  contactsRoute() {
    if (this.variablesService.appPass) {
      this.ngZone.run(() => { this.router.navigate(['/contacts']) })

    } else {
      this.modal.prepareModal(
        'error',
        'CONTACTS.FORM_ERRORS.SET_MASTER_PASSWORD'
      );
    }
  }

  drop(event: CdkDragDrop<Wallet[]>) {
    moveItemInArray(this.variablesService.wallets, event.previousIndex, event.currentIndex)
  }
  showDialog(wallet_id) {
    this.isModalDialogVisible = true;
    this.closeWalletId = wallet_id;
  }

  confirmed(confirmed: boolean) {
    if (confirmed) {
      this.closeWallet(this.closeWalletId);
    }
    this.isModalDialogVisible = false;
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

  getUpdate() {
    this.backend.openUrlInBrowser(DOWNLOADS_PAGE_URL);
  }
  goToAuditableWalletHelpPage(e) {
    e.preventDefault();
    this.backend.openUrlInBrowser(AUDITABLE_WALLET_HELP_PAGE);
  }

  logOut() {
    this.variablesService.stopCountdown();
    this.variablesService.appLogin = false;
    this.variablesService.appPass = '';
    this.ngZone.run(() => {
      this.router.navigate(['/login'], { queryParams: { type: 'auth' } });
    });
  }

  ngOnDestroy() {

  }
}
