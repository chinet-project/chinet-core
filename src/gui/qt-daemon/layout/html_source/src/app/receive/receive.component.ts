import { Component, OnInit, OnDestroy } from '@angular/core';
import QRCode from 'qrcode';
import { BackendService } from '../_helpers/services/backend.service';
import { VariablesService } from '../_helpers/services/variables.service';
import { RCV_ADDR_QR_SCALE } from '../_shared/constants';

@Component({
  selector: 'app-receive',
  templateUrl: './receive.component.html',
  styleUrls: ['./receive.component.scss']
})
export class ReceiveComponent implements OnInit, OnDestroy {
  qrImageSrc: string;
  parentRouting;
  copyAnimation = false;
  copyAnimationTimeout;

  constructor(
    private backend: BackendService,
    public variablesService: VariablesService
  ) { }

  ngOnInit() {
    QRCode.toDataURL(this.variablesService.currentWallet.address, {
      width: 200 * RCV_ADDR_QR_SCALE,
      height: 200 * RCV_ADDR_QR_SCALE
    }).then(url => {
      this.qrImageSrc = url;
    }).catch(err => {
      console.error(err);
    });
  }

  public copyAddress() {
    this.backend.setClipboard(this.variablesService.currentWallet.address);
    this.copyAnimation = true;
    this.copyAnimationTimeout = window.setTimeout(() => {
      this.copyAnimation = false;
    }, 2000);
  }

  ngOnDestroy() {
    clearTimeout(this.copyAnimationTimeout);
  }

}
