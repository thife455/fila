export default class Client {
    index: number

    constructor(idx: number){
        this.index = idx
    }
    updateIndex(newIndex: number){
        this.index = newIndex;
    }
}