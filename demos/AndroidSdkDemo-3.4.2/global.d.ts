import { tdsdk } from "./assets/script/tdsdk";
declare global {
    interface Window {
        tdsdk: tdsdk
    }
}