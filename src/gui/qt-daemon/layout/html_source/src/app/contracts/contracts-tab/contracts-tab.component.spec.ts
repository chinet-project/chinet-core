import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { ContractsTabComponent } from './contracts-tab.component';

describe('ContractsTabComponent', () => {
  let component: ContractsTabComponent;
  let fixture: ComponentFixture<ContractsTabComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ ContractsTabComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ContractsTabComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
