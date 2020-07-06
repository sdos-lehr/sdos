import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import {CollectionPickerComponent} from "./collection-picker/collection-picker.component";
import {CollectionComponent} from "./collection/collection.component";
import {EntryComponent} from "./entry/entry.component";


const routes: Routes = [
  {path: '', redirectTo: 'collection-list', pathMatch: 'full'},
  {path: 'collection-list', component: CollectionPickerComponent},
  {path: 'collection/:cid', component: CollectionComponent},
  {path: 'collection/:cid/:eid', component: EntryComponent}
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
