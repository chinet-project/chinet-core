import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { SendDetailsModalComponent } from './send-details-modal.component';

describe('SendDetailsModalComponent', () => {
  let component: SendDetailsModalComponent;
  let fixture: ComponentFixture<SendDetailsModalComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ SendDetailsModalComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(SendDetailsModalComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
