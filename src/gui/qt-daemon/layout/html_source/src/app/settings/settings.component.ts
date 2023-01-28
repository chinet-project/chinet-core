import {Component, NgZone, OnInit, Renderer2} from '@angular/core';
import {VariablesService} from '../_helpers/services/variables.service';
import {BackendService} from '../_helpers/services/backend.service';
import {FormControl, FormGroup, Validators} from '@angular/forms';
import {Location} from '@angular/common';
import {TranslateService} from '@ngx-translate/core';
import {UtilsService} from '../_helpers/services/utils.service';

@Component({
  selector: 'app-settings',
  templateUrl: './settings.component.html',
  styleUrls: ['./settings.component.scss'],
  providers: [UtilsService]
})
export class SettingsComponent implements OnInit {
  ifSaved = false;
  scale: number;
  appUseTor: boolean;
  changeForm: any;
  public currentNotificationsState;
  languagesOptions = [
    {
      name: 'en',
      language: 'SETTINGS.LANGUAGE.EN'
    },
    {
      name: 'fr',
      language: 'SETTINGS.LANGUAGE.FR'
    },
    {
      name: 'de',
      language: 'SETTINGS.LANGUAGE.DE'
    },
    {
      name: 'id',
      language: 'SETTINGS.LANGUAGE.ID'
    },
    {
      name: 'it',
      language: 'SETTINGS.LANGUAGE.IT'
    },
    {
      name: 'pt',
      language: 'SETTINGS.LANGUAGE.PT'
    }
  ];
  appLockOptions = [
    {
      id: 5,
      name: 'SETTINGS.APP_LOCK.TIME1'
    },
    {
      id: 15,
      name: 'SETTINGS.APP_LOCK.TIME2'
    },
    {
      id: 60,
      name: 'SETTINGS.APP_LOCK.TIME3'
    },
    {
      id: 0,
      name: 'SETTINGS.APP_LOCK.TIME4'
    }
  ];
  appScaleOptions = [
    {
      id: 6,
      name: 'SETTINGS.SCALE.75'
    },
    {
      id: 8,
      name: 'SETTINGS.SCALE.100'
    },
    {
      id: 10,
      name: 'SETTINGS.SCALE.125'
    },
    {
      id: 12,
      name: 'SETTINGS.SCALE.150'
    }
  ];
  appLogOptions = [
    {
      id: -1
    },
    {
      id: 0
    },
    {
      id: 1
    },
    {
      id: 2
    },
    {
      id: 3
    },
    {
      id: 4
    }
  ];

  currentBuild = '';
  appPass: any;

  constructor(
    private renderer: Renderer2,
    public variablesService: VariablesService,
    private backend: BackendService,
    private location: Location,
    public translate: TranslateService,
    private ngZone: NgZone,
    private utilsService: UtilsService
  ) {
    this.scale = this.variablesService.settings.scale;
    this.appUseTor = this.variablesService.settings.appUseTor;
    this.changeForm = new FormGroup({
      password: new FormControl(''),
      new_password: new FormControl('', Validators.pattern(this.variablesService.pattern)),
      new_confirmation: new FormControl('')
    }, [(g: FormGroup) => {
      return g.get('new_password').value === g.get('new_confirmation').value ? null : {'confirm_mismatch': true};
    }, (g: FormGroup) => {
      if (this.variablesService.appPass) {
        return g.get('password').value === this.variablesService.appPass ? null : {'pass_mismatch': true};
      }
      return null;
    }]);
  }

  ngOnInit() {
    this.backend.getVersion((version, type) => {
      this.ngZone.run(() => {
        this.currentBuild = version;
        this.variablesService.testnet = false;
        if (type === 'testnet') {
          this.currentBuild += ' TESTNET';
          this.variablesService.testnet = true;
        }
        this.variablesService.networkType = type;
      });
    });
    this.backend.getIsDisabledNotifications((res) => {
      this.currentNotificationsState = res;
    });
  }

  setScale() {
    this.scale = this.variablesService.settings.scale;
    const width = this.utilsService.getMinWidthByScale(this.scale);
    const app = document.documentElement.querySelector('app-root');
    this.renderer.setStyle(app, 'min-width', width + 'px');
    this.renderer.setStyle(document.documentElement, 'font-size', this.scale + 'px');
    this.backend.storeAppData();
  }

  onSubmitChangePass() {
    if (this.changeForm.valid) {
      this.onSave();
      this.variablesService.appPass = this.changeForm.get('new_password').value;
      if (this.variablesService.appPass) {
        this.backend.setMasterPassword({pass: this.variablesService.appPass}, (status, data) => {
          if (status) {
            this.backend.storeSecureAppData({pass: this.variablesService.appPass});
            this.variablesService.appLogin = true;
            this.variablesService.dataIsLoaded = true;
            if (this.variablesService.settings.appLockTime) {
              this.variablesService.startCountdown();
            }
          } else {
            console.log(data['error_code']);
          }
        });
      } else {
        // this.backend.dropSecureAppData((status, data) => {
        // });
      }
      this.changeForm.reset();
    }
  }

  toggleNotifications() {
    if (!this.currentNotificationsState) {
      this.backend.setIsDisabledNotifications('true');
      this.currentNotificationsState = true;
    } else {
      this.backend.setIsDisabledNotifications('false');
      this.currentNotificationsState = false;
    }
  }

  toggleUseTor() {
    this.appUseTor = !this.appUseTor;
    this.variablesService.settings.appUseTor = this.appUseTor;
    this.backend.setEnableTor(this.appUseTor);
    this.backend.storeAppData();
  }

  onSave() {
    this.ifSaved = true;
    setTimeout(() => {
      this.ifSaved = false;
    }, 3000);
  }

  onLockChange() {
    if (this.variablesService.appLogin && this.variablesService.settings.appLockTime) {
      this.variablesService.restartCountdown();
    }
  }

  onLogChange() {
    this.backend.setLogLevel(this.variablesService.settings.appLog);
    this.backend.storeAppData();
  }

  onLanguageChange() {
    this.translate.use(this.variablesService.settings.language);
    this.backend.storeAppData();
  }

  back() {
    this.location.back();
  }

}
