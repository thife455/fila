"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const express_1 = __importDefault(require("express"));
const socket_io_1 = require("socket.io");
const connectionHandler_1 = __importDefault(require("../entities/connectionHandler"));
const express_session_1 = __importDefault(require("express-session"));
const redis_1 = __importDefault(require("redis"));
let RedisStore = require("connect-redis")(express_session_1.default);
const app = express_1.default();
const http = require("http");
const server = http.createServer(app);
const io = new socket_io_1.Server(server);
const handler = new connectionHandler_1.default();
let redisClient = redis_1.default.createClient();
app.use(express_session_1.default({
    secret: "keyboard cat",
    resave: false,
    saveUninitialized: true,
    store: new RedisStore({ client: redisClient }),
    cookie: { maxAge: 6000000 },
}));
let index = 0;
app.get("/", function (req, res) {
    res.sendFile(__dirname + "/index.html");
    if (req.session.index) {
    }
    else {
        req.session.index = handler.clientOrder.length + 1;
        handler.createClient(req.session.index);
    }
    index = req.session.index;
    console.log(req.session.index);
});
io.on("connection", (socket) => {
    console.log("user connected");
    socket.emit("hello", index);
    socket.on("disconnect", () => {
        console.log("user disconnected");
    });
});
server.listen(3000, () => {
    console.log("listening on *:3000");
});
//# sourceMappingURL=index.js.map