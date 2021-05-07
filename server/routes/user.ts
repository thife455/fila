import express from "express";

const userRouter = express.Router();

userRouter.get("/", (_req, res) => {
  res.send("hello");
});

export default userRouter;
