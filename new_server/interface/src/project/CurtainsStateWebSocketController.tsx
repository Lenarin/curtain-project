import { CurtainState, CurtainsState } from "./types";
import { WebSocketControllerProps, WebSocketFormLoader, WebSocketFormProps, webSocketController } from '../components';
import React, { Component, useEffect, useState } from "react";
import { ValidatorForm } from "react-material-ui-form-validator";
import { WEB_SOCKET_ROOT } from '../api';
import { Button, Dialog, DialogActions, DialogContent, Divider, Grid, IconButton, Paper, Slider, TextField, Typography, useMediaQuery, useTheme } from "@material-ui/core";
import SettingsIcon from '@material-ui/icons/Settings';
import DialogTitle from '@material-ui/core/DialogTitle';
import RefreshIcon from '@material-ui/icons/Refresh';

export const CURTAINS_SETTINGS_WEBSOCKET_URL = WEB_SOCKET_ROOT + "curtainsState";

type CurtainsStateWebSocketControllerProps = WebSocketControllerProps<CurtainsState>;
type CurtainsStateWebSocketControllerFormProps = WebSocketFormProps<CurtainsState>;

class CurtainsStateWebSocketController extends Component<CurtainsStateWebSocketControllerProps> {

    render() {
        return (
            <WebSocketFormLoader
                {...this.props}
                render={props => (
                    <CurtainsStateWebSocketControllerForm {...props} />
                )}
            />
        )
    }
}

function CurtainsStateWebSocketControllerForm(props: CurtainsStateWebSocketControllerFormProps) {
    const { data, saveData, setData, enqueueSnackbar, closeSnackbar } = props;

    const theme = useTheme();
    const fullScreen = useMediaQuery(theme.breakpoints.down('sm'));

    const [dialogOpen, setDialogOpen] = useState<boolean>(false);
    const [settingsDialogCurtain, setSettingsDialogCurtain] = useState<CurtainState|undefined>(undefined);
    const [settingName, setSettingName] = useState<string>("");
    const [settingMaxPosition, setSettingMaxPosition] = useState<number>(0);
    const [settingMinPosition, setSettingMinPosition] = useState<number>(0);

    const [curtains, setCurtains] = useState<CurtainState[]>(data.curtains);

    useEffect(() => {
        if (data.inScan === false) {
            closeSnackbar();
        }
        if (data.command === "set") {
            setCurtains(data.curtains);
        }
    }, [data])

    const handleNewPosition = (position: number, guid: string) => {
        const newData = curtains.find(el => el.guid === guid);
        if (newData) {
            newData.position = position;

            setData({curtains: [newData]}, saveData);
        }
    }

    const handleDialogClose = () => {
        setDialogOpen(false);
    }

    const handleDialogOpen = (curtain: CurtainState) => {
        setSettingName(curtain.name);
        setSettingMaxPosition(curtain.maxPosition);
        setSettingMinPosition(curtain.minPosition);
        setSettingsDialogCurtain(curtain);
        setDialogOpen(true);
    }

    const handleSettingsDialogSubmit = () => {
        const newData = settingsDialogCurtain;
        if (newData) {
            newData.name = settingName;
            newData.maxPosition = settingMaxPosition;
            newData.minPosition = settingMinPosition;

            setData({curtains: [newData]}, saveData);
        }


        handleDialogClose();
    }

    const handleRescanButtonClick = () => {
        enqueueSnackbar("Сканируем устройства...");
        setData({curtains: [], command: "refresh", inScan: true}, saveData);
    }

    const handleOpenAllButtonClick = () => {
        const newCurtains = curtains.map(el => {el.position = el.maxPosition; return el;});
        setData({curtains: newCurtains}, saveData);

    }

    const handleCloseAllButtonClick = () => {
        const newCurtains = curtains.map(el => {el.position = el.minPosition; return el;});
        setData({curtains: newCurtains}, saveData);
    }

    return (
        <ValidatorForm onSubmit={saveData}>
            <Grid container direction="column" spacing={0} >
                <Grid container item style={{padding: 18, paddingBottom: 0}} justify="space-around">
                    <Button variant="contained" color="primary" onClick={handleRescanButtonClick}>
                        <RefreshIcon />
                        Сканировать
                    </Button>
                    <Button variant="contained" color="primary" onClick={handleCloseAllButtonClick}>
                        Закрыть все
                    </Button>
                    <Button variant="contained" color="primary" onClick={handleOpenAllButtonClick}>
                        Открыть все
                    </Button>
                </Grid>
                {curtains.map(curtain => (
                    <CurtainCard
                        key = {curtain.guid}
                        curtain = {curtain}
                        onNewPosition = {handleNewPosition}
                        onSettingsButtonClick = {handleDialogOpen}
                    />
                ))}
            </Grid>
            <Dialog
                open={dialogOpen}
                onClose={handleDialogClose}
                fullScreen={fullScreen}
                fullWidth
            >
                <DialogTitle id="settings-dialog-title">Настройки</DialogTitle>
                <DialogContent>
                    <TextField id="name" label="Название" type="text" fullWidth margin="dense"
                        value={settingName} onChange={(e: React.ChangeEvent<HTMLTextAreaElement | HTMLInputElement>) => setSettingName(e.target.value)}
                    />
                    <TextField id="maxPosition" label="Масимум" type="number" fullWidth margin="dense"
                        value={settingMaxPosition} onChange={(e: React.ChangeEvent<HTMLTextAreaElement | HTMLInputElement>) => setSettingMaxPosition(Number(e.target.value))}
                    />
                    <TextField id="minPosition" label="Минимум" type="number" fullWidth margin="dense"
                        value={settingMinPosition} onChange={(e: React.ChangeEvent<HTMLTextAreaElement | HTMLInputElement>) => setSettingMinPosition(Number(e.target.value))}
                    />
                </DialogContent>
                <DialogActions>
                    <Button color="primary" onClick={handleDialogClose}>
                        Отмена
                    </Button>
                    <Button color="primary" onClick={handleSettingsDialogSubmit}>
                        Сохранить
                    </Button>
                </DialogActions>
            </Dialog>
        </ValidatorForm>
    )
}

type CurtainCardProps = {
    curtain: CurtainState,
    onNewPosition: (position: number, guid: string) => void
    onSettingsButtonClick: (curtain: CurtainState) => void;
}

function CurtainCard(props: CurtainCardProps) {
    const {curtain, onNewPosition, onSettingsButtonClick} = props;

    const [position, setPosition] = useState<number>(curtain.position);

    useEffect(() => {
        setPosition(curtain.position);
    }, [curtain.position])

    const handlePositionChange = (event: React.ChangeEvent<{}>, newValue: number | number[]) => {
        setPosition(Number(newValue));
    }

    const handleChangeCommited = (event: React.ChangeEvent<{}>, newValue: number | number[]) => {
        onNewPosition(Number(newValue), curtain.guid)
    }

    return (
        <Paper style={{margin: 12, padding: 6}} id={curtain.guid}>
            <Grid container direction="column">
                <Grid item container justify="space-around" direction="row" wrap="nowrap">
                    <Grid item container alignItems="flex-start" direction="column">
                        <Typography variant="h5">
                            {`${curtain.name}  `}
                        </Typography>
                        <Typography variant="subtitle2" style={{verticalAlign: "bottom"}}>
                            {curtain.guid}
                        </Typography>
                    </Grid>
                    <IconButton onClick={() => onSettingsButtonClick(curtain)}>
                        <SettingsIcon />
                    </IconButton>
                </Grid>
                <Divider style={{marginBottom: 8}} />
                <Typography >
                    Позиция
                </Typography>
                <Slider
                    value={position}
                    onChange={handlePositionChange}
                    aria-labelledby="input-slider"
                    min={curtain.minPosition}
                    max={curtain.maxPosition}
                    onChangeCommitted={handleChangeCommited}
                />
            </Grid>
        </Paper>
    )
}

export default webSocketController(CURTAINS_SETTINGS_WEBSOCKET_URL, 100, CurtainsStateWebSocketController);