/**
 * Renix ECU De-stuffer and Parser
 * Processes raw UART bytes, handles byte stuffing, and extracts 30-byte packets.
 */
export class RenixDestuffer {
  static PACKET_MAX_SIZE = 64; // cannot store larger packets
  static PACKET_MIN_SIZE = 30; // reject smaller packets
  private packetBuffer = new Uint8Array(RenixDestuffer.PACKET_MAX_SIZE); // Working buffer for one packet
  private writeIndex = 0;
  private packetCount = 0;
  private lastWas255 = false;

  /**
   * @param onPacket Callback called for each valid 30-byte packet.
   */
  constructor(private onPacket: (packet: Uint8Array, count: number) => void) {}

  /**
   * Process a new chunk of raw data from the ECU.
   */
  public process_chunk(chunk: Uint8Array): void {
    for (let i = 0; i < chunk.length; i++) {
      const currentByte = chunk[i];

      if (this.writeIndex >= RenixDestuffer.PACKET_MAX_SIZE) {
        this.writeIndex = 0;
        this.lastWas255 = false;
        return; // malformatted input data
      }

      if (this.lastWas255) {
        if (currentByte === 255) {
          // Rule 1: 255, 255 is a STUFFED DATA BYTE
          this.appendToPacket(255);
          this.lastWas255 = false;
        } else {
          // Rule 2: 255 followed by [Not 255] is the FRAME DELIMITER.
          // The currentByte is the START BYTE (0) of the NEW packet.
          if (this.writeIndex >= RenixDestuffer.PACKET_MIN_SIZE) this.finalizePacket();

          // Reset for new packet and start with the byte that followed the delimiter
          this.writeIndex = 0;
          this.appendToPacket(currentByte);
          this.lastWas255 = false;
        }
      } else {
        if (currentByte === 255) {
          this.lastWas255 = true;
        } else {
          this.appendToPacket(currentByte);
        }
      }
    }
  }

  private appendToPacket(byte: number): void {
    if (this.writeIndex < this.packetBuffer.length) {
      this.packetBuffer[this.writeIndex++] = byte;
    }
  }

  private finalizePacket(): void {
    this.packetCount++;
    //this.onPacket(this.packetBuffer.slice(0, this.writeIndex - 1), this.packetCount);
    this.onPacket(this.packetBuffer.subarray(0, this.writeIndex - 1), this.packetCount);
  }
}
