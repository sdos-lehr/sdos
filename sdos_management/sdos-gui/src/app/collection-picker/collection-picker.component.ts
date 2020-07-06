import {Component, OnInit} from '@angular/core';

@Component({
  selector: 'app-collection-picker',
  templateUrl: './collection-picker.component.html',
  styleUrls: ['./collection-picker.component.scss']
})
export class CollectionPickerComponent implements OnInit {
  collections = [];
  private localStorageKey;
  collectionName: string;

  constructor() {
  }

  private readCollections() {
    const storedCollections = JSON.parse(localStorage.getItem(this.localStorageKey));
    if (storedCollections && storedCollections.length > 0) {
      this.collections = storedCollections;
    }
  }

  ngOnInit(): void {
    this.readCollections();
  }

  addCollection(name: string) {
    if (!CollectionPickerComponent.nameValid(name) || this.nameExists(name)) {
      console.warn('name ' + name + ' not valid.');
      return;
    }
    this.collections.push(name);
    localStorage.setItem(this.localStorageKey, JSON.stringify(this.collections));
    console.log('Collection', name, 'added. List now is', this.collections);
  }

  clearCollections() {
    this.collections = [];
    localStorage.removeItem(this.localStorageKey);
  }

  private static nameValid(name: string): boolean {
    if (!name) {
      return false;
    }
    if (name.trim() !== name) {
      console.warn('name contains whitespaces');
      return false;
    }
    if (name.length <= 1) {
      console.warn('name must be at least 2 characters long');
      return false;
    }
    if (name.indexOf('.') >= 0) {
      console.warn('name contains dots');
      return false;
    }
    return true;
  }

  private nameExists(name: string): boolean {
    return this.collections.indexOf(name) >= 0;
  }
}
