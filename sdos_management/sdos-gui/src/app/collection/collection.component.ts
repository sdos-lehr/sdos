import {Component, OnInit} from '@angular/core';
import {ApiService} from "../api.service";
import {ActivatedRoute, Router} from "@angular/router";
import * as uuid from 'uuid';

@Component({
  selector: 'app-collection',
  templateUrl: './collection.component.html',
  styleUrls: ['./collection.component.scss']
})
export class CollectionComponent implements OnInit {
  collectionID: string = '';
  constructor(private route: ActivatedRoute,
              private router: Router,
              public api: ApiService) {
  }

  ngOnInit(): void {
    this.route.params.subscribe(params => {
      this.collectionID = params['cid'];
      this.api.fetchCollectionEntries(this.collectionID);
    })
  }

  async addEntry(): Promise<void> {
    const id = uuid.v4();
    console.log('generated id:', id);
    const route = 'collection/' + this.collectionID + '/' + id;
    this.api.baseEntry = undefined;
    await this.router.navigate([route]);
  }
}
