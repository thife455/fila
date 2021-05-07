import Client from "./client";

export default class connectionHandler {
  clientOrder: Array<Client> = [];

  createClient(index: number) {
    const tempClient = new Client(index);
    this.clientOrder.push(tempClient);
    console.log(this.clientOrder)
    return tempClient.index;
  }
  deleteClient(index: number) {
    this.clientOrder.splice(index)
    // this.clientOrder.forEach((client) => {
    //   console.log(client);
    //   client.updateIndex(this.clientOrder.indexOf(client));
    // });
  }
}
