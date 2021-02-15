export interface LightState {
  led_on: boolean;
}

export interface LightMqttSettings {
  unique_id : string;
  name: string;
  mqtt_path : string;
}

export interface CurtainState {
  name: string;
  guid: string;
  position: number;
  maxPosition: number;
  minPosition: number;
}

export interface CurtainsState {
  curtains: CurtainState[];
  inScan?: boolean;
  command?: string;
}
