import React, { Component } from 'react';
import { RouteComponentProps } from 'react-router-dom';
import { MenuAppBar } from '../components';
import CurtainsStateWebSocketController from './CurtainsStateWebSocketController';

class CurtainsProject extends Component<RouteComponentProps> {


    render() {
        return (
            <MenuAppBar sectionTitle="Curtains Project">
                <CurtainsStateWebSocketController />
            </MenuAppBar>
        )
    }
}

export default CurtainsProject;