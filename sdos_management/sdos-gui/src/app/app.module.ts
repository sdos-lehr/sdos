import {BrowserModule} from '@angular/platform-browser';
import {NgModule} from '@angular/core';

import {AppRoutingModule} from './app-routing.module';
import {AppComponent} from './app.component';
import {CollectionPickerComponent} from './collection-picker/collection-picker.component';
import {CollectionComponent} from './collection/collection.component';
import {EntryComponent} from './entry/entry.component';
import {BrowserAnimationsModule} from '@angular/platform-browser/animations';
import {MatListModule} from "@angular/material/list";
import {FormsModule} from "@angular/forms";
import {MatCommonModule} from "@angular/material/core";
import {MatFormFieldModule} from "@angular/material/form-field";
import {MatInputModule} from "@angular/material/input";
import {MatIconModule} from "@angular/material/icon";
import {MatButtonModule} from "@angular/material/button";
import {MatToolbarModule} from "@angular/material/toolbar";
import {MatTooltipModule} from "@angular/material/tooltip";
import {HttpClient, HttpClientModule} from "@angular/common/http";
import {NgJsonEditorModule} from "ang-jsoneditor";

@NgModule({
  declarations: [
    AppComponent,
    CollectionPickerComponent,
    CollectionComponent,
    EntryComponent
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    BrowserAnimationsModule,
    HttpClientModule,
    MatListModule,
    FormsModule,
    MatCommonModule,
    MatFormFieldModule,
    MatInputModule,
    MatIconModule,
    MatButtonModule,
    MatToolbarModule,
    MatTooltipModule,
    NgJsonEditorModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule {
}
