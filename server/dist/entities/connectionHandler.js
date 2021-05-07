"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const client_1 = __importDefault(require("./client"));
class connectionHandler {
    constructor() {
        this.clientOrder = [];
    }
    createClient(index) {
        const tempClient = new client_1.default(index);
        this.clientOrder.push(tempClient);
        console.log(this.clientOrder);
        return tempClient.index;
    }
    deleteClient(index) {
        this.clientOrder.splice(index);
    }
}
exports.default = connectionHandler;
//# sourceMappingURL=connectionHandler.js.map