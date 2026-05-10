import { raw_parser } from "../parser/raw";
import type { Icar_chart, Icar_chart_static, ILabel } from "./iface";
export type { Icar_chart, ILabel };

const labels: readonly ILabel[] = (() => {
  const l: ILabel[] = [];
  for (let g = 0; g <= 64; ++g) {
    l.push({ series_label: `B${g}`, axis_label: "raw", range: [0, 255] });
  }
  return l;
})();

export class raw_chart implements Icar_chart {
  public nmbGraphs: number = $state(20);
  public labels = $derived(labels.slice(0, this.nmbGraphs));
  public order = $derived([...Array(this.nmbGraphs).keys()]);
  public packets_per_sec = $state(41);

  private yn_arr: (number | boolean)[][] = Array.from({ length: 64 }, () => []);
  static get_info() {
    return { name: "Raw", description: "Generic. Show each data-byte as a 0...255 graph" };
  }
  clear_chart_data() {
    this.yn_arr = Array.from({ length: 64 }, () => []);
    //this.yn_arr.forEach((subArray) => (subArray.length = 0));
  }

  get_car_metrics(): Array<CarMetrics> {
    return [];
  }

  get_chart_data() {
    return this.yn_arr;
  }
  get_nmb_of_graphs() {
    return this.nmbGraphs;
  }

  get_labels() {
    return this.labels;
  }

  get_label(n) {
    return this.labels[n];
  }

  process_data_packet(arr: Uint8Array, ct: number) {
    if (!this.yn_arr) return;
    this.nmbGraphs =  arr.length;
    const m = new raw_parser(arr);
    let idx = 0;
    for (let g = 0; g < this.nmbGraphs; ++g) {
      console.assert(this.yn_arr.length >= this.nmbGraphs);
      this.yn_arr[idx++].push(m.get_byte(g));
    }
    return true;
  }
}

raw_chart satisfies Icar_chart_static;
