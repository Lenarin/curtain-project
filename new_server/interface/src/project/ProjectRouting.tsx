import React, { Component } from 'react';
import { Redirect, Switch } from 'react-router';

import { PROJECT_PATH } from '../api';
import { AuthenticatedRoute } from '../authentication';

import CurtainsProject from './CurtainsProject';

class ProjectRouting extends Component {

  render() {
    return (
      <Switch>
        <AuthenticatedRoute exact path={`/${PROJECT_PATH}/demo/*`} component={CurtainsProject} />
        <Redirect to={`/${PROJECT_PATH}/demo/`} />
      </Switch>
    )
  }

}

export default ProjectRouting;
