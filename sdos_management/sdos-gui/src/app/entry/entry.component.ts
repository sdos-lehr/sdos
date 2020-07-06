import {Component, OnInit, ViewChild} from '@angular/core';
import {ActivatedRoute, Router} from "@angular/router";
import {HttpClient} from "@angular/common/http";
import {ApiService} from "../api.service";
import {JsonEditorComponent, JsonEditorOptions} from "ang-jsoneditor";
import * as uuid from 'uuid';

@Component({
  selector: 'app-entry',
  templateUrl: './entry.component.html',
  styleUrls: ['./entry.component.scss']
})
export class EntryComponent implements OnInit {
  public collectionID: string = '';
  public entryID: string = '';
  public editorOptions: JsonEditorOptions;
  data: object = {};
  changedData: object = {};

  constructor(private route: ActivatedRoute, private router: Router, private api: ApiService) {
    this.editorOptions = new JsonEditorOptions();
    this.editorOptions.modes = ['code', "text", "view"];
  }

  ngOnInit(): void {
    this.route.params.subscribe(params => {
      this.collectionID = params['cid'];
      this.entryID = params['eid'];
      this.loadData();
    });
  }

  private loadData(): void {
    if (this.api.baseEntry) {
      this.data = {...this.api.baseEntry};
      this.changedData = this.data;
      this.api.baseEntry = undefined;
    } else {
      this.api.fetchEntryData(this.collectionID, this.entryID).subscribe(data => {
        console.log(data);
        this.data = data;
        this.changedData = this.data;
      });
    }
  }

  private persistData(): void {

  }

  updateData(data: any) {
    if (data) {
      this.changedData = data;
    }
  }

  async addEntry() {
    const id = uuid.v4();
    console.log('generated id:', id);
    const route = 'collection/' + this.collectionID + '/' + id;
    this.api.baseEntry = this.changedData;
    await this.router.navigate([route]);
  }

  saveEntry() {
    console.log('saving entry');
    this.api.persistEntry(this.collectionID, this.entryID, this.changedData).subscribe(resp => {
      console.log(resp);
      this.data = this.changedData;
    }, err => {
      console.log('An error occured wile saving entry ' + this.entryID, err);
    });
  }
}
