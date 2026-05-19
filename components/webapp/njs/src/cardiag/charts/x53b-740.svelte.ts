import { x53b_740_parser, x53b_740_metrics_table, x53b_740_metrics_table_pos } from "../parser/x53b-740";
import type { CarMetrics, Icar_chart_static, Icar_chart, ILabel, IModelInfo } from "./iface";
export type { Icar_chart, ILabel };

const t = x53b_740_metrics_table;
const e = x53b_740_metrics_table_pos;
const default_order = [
  e.engine_speed,
  e.idle_switch,

  e.map_sensor,
  e.fuel_pump_relay,

  e.ignition_advance,
  e.full_load_switch,

  e.injection_duration,
  e.batt_voltage,

  e.o2_sensor,
  e.is_o2_closed_loop,

  e.richness_regulation,
  e.is_o2_rich,

  e.richness_adaption_idle2low,
  e.richness_adaption_avg2high,

  e.ect_sensor,
  e.iat_sensor,

  e.engine_pinking,
  e.detonation_correction,

  e.idle_regulation,
  e.idle_adaption,

  e.status0,
  e.status1,

  e.status2,
  e.status3,

  e.status4,
  e.status5,

  e.status6,
  e.status7,

  e.unknown_x,
  e.unknown_xx,

];

export class x53b_740_chart implements Icar_chart {
  private yn_all_arr: (number | boolean)[][] = $state.raw(Array.from({ length: t.length }, () => []));
  public order = $state(default_order);
  public nmbGraphs: number = $derived(this.order.length);
  public packets_per_sec = $state(41);

  public labels = $derived.by(() => {
    const len = this.order.length;
    const labels: Array<ILabel> = new Array<ILabel>(len);
    for (let i = 0; i < len; ++i) {
      const te = t[this.order[i]];
      labels[i] = { series_label: te.short_name, axis_label: te.unit, range: te.range };
    }
    return labels;
  }) as ILabel[];

  public static get_info(): IModelInfo {
    return { name: "X53B_740", description: "Model R19-F3N740 (54kW, TBI, manual)" };
  }

  clear_chart_data() {
    console.log("clear chart data");
    this.yn_all_arr = Array.from({ length: t.length }, () => []);
    //this.yn_all_arr.forEach(subArray => subArray.length = 0);
  }

  get_chart_data() {
    let arr: (number | boolean)[][] = Array.from({ length: this.order.length }, () => []);
    for (let i = 0; i < this.order.length; ++i) {
      arr[i] = this.yn_all_arr[this.order[i]];
    }
    return arr;
  }
  get_labels() {
    return this.labels;
  }
  get_label(n: number) {
    return this.labels[n];
  }
  get_nmb_of_graphs() {
    return this.order.length;
  }

  get_car_metrics(): Array<CarMetrics> {
    return [...x53b_740_metrics_table];
  }

  process_data_packet(arr: Uint8Array, ct: number) {
    const m = new x53b_740_parser(arr);
    let idx = 0;
    for (let i = 0; i < t.length; ++i) {
      this.yn_all_arr[i].push(t[i].parse.call(m));
    }
  }
}

export function x53b_740_chart_factory() {
  return new x53b_740_chart() as Icar_chart;
}

x53b_740_chart satisfies Icar_chart_static;
