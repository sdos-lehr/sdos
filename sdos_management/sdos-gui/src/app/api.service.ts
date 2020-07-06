import {Injectable} from '@angular/core';
import {HttpClient} from '@angular/common/http';
import {Observable} from "rxjs";

@Injectable({
  providedIn: 'root'
})
export class ApiService {
  public lastCollection: string;
  public lastEntries: string[];
  public baseEntry: any = undefined;
  private base: string = 'http://localhost:5006/?collection=';
  private options: { headers: { "content-type": string }} = {
    headers: {
      'content-type': 'application/json'
    }
  };

  constructor(private http: HttpClient) {
  }

  public fetchCollectionEntries(collection: string): void {
    const url = this.base + collection;
    this.http.get(url, {
      headers: {
        'content-type': 'application/json'
      },
      responseType: 'json'
    }).subscribe(resp => {
      this.lastCollection = collection;
      console.log(resp);
      this.lastEntries = resp['ids'];
    });
  }

  public fetchEntryData(collection: string, entryId: string): Observable<any> {
    const url = this.base + collection + '&id=' + entryId;
    return this.http.get(url, this.options);
  }

  public persistEntry(collection: string, entryId: string, data: any): Observable<any> {
    const url = this.base + collection + '&id=' + entryId;
    return this.http.post(url, data, this.options);
  }
}
