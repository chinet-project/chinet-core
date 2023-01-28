import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { DeeplinkComponent } from './deeplink.component';

describe('DeeplinkComponent', () => {
  let component: DeeplinkComponent;
  let fixture: ComponentFixture<DeeplinkComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ DeeplinkComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(DeeplinkComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
