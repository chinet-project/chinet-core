import { Component, OnInit, OnDestroy, NgZone } from '@angular/core';
import { Location } from '@angular/common';
import { VariablesService } from '../_helpers/services/variables.service';
import { ActivatedRoute, Router } from '@angular/router';


@Component({
  selector: 'app-contact-send',
  templateUrl: './contact-send.component.html',
  styleUrls: ['./contact-send.component.scss']
})
export class ContactSendComponent implements OnInit, OnDestroy {

  queryRouting;
  address;

  constructor(
    private location: Location,
    public variablesService: VariablesService,
    private route: ActivatedRoute,
    private ngZone: NgZone,
    private router: Router,
  ) { }

  ngOnInit() {
    this.queryRouting = this.route.queryParams.subscribe(params => {
      if (params.address) {
        this.address = params.address;
      }
    });
  }

  goToWallet(id) {
    this.variablesService.setCurrentWallet(id);
    this.variablesService.currentWallet.send_data['address'] = this.address;
    this.ngZone.run(() => {
      this.router.navigate(['/wallet/send'], { queryParams: { send: true } });
    });
  }

  back() {
    this.location.back();
  }

  ngOnDestroy() {
    this.queryRouting.unsubscribe();
  }

}
