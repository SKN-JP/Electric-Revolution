#include"Game.h"

void	Game::updateFactoryView()
{
	auto* f = mainFactory;
	auto& ui = f->ui;

	//カメラを更新
	f->camera.update();

	Array<std::thread>	threads;
	for (auto& u : planet.urbans)
	{
		auto* tf = &u.factory;
		threads.emplace_back([this, tf]() { updateFactory(tf); });
	}

	for (auto& t : threads) t.join();

	//シーン移行
	if (KeyEscape.down()) sceneState = SceneState::MapView;

	{
		//視点移動を適用
		auto t = f->camera.createTransformer();

		//機械の音量制御
		for (auto& m : f->machines)
		{
			if (!m.enabled) continue;
			auto dr = f->camera.getDrawingRegion();
			Vec3 pos1(dr.center().x, dr.center().y, dr.w / 2.0);
			Vec3 pos2(m.region().center().x, m.region().center().y, 0.0);
			for (auto& a : m.audioAssets)
			{
				a.second.setVolume(Min(0.5, 100.0 / (pos1 - pos2).lengthSq()));
			}
		}

		//Tilesの描画
		{
			RectF r = f->camera.getDrawingRegion();
			for (auto p : step(Point(int(r.x), int(r.y)), Size(int(r.w) + 2, int(r.h) + 2)))
			{
				auto* tile = f->getTile(p);
				if (tile != nullptr) f->tileTexture(tile->type * 64, 0, 64, 64).resized(1.0, 1.0).draw(p);
			}
		}

		//Wire系の描画
		if (ui.uiMode != UIMode::EditWireMode)
		{
			for (auto& w : f->wires) w.getBezier3().draw(1 / 8.0, w.color);
		}

		//Machinesの描画
		for (auto& m : f->machines)
		{
			if (m.enabled)
			{
				if (m.lua["draw"].get_type() == sol::type::function) m.lua["draw"]();

				if (ui.uiMode == UIMode::EditMachineMode && !f->ui.mouseOver)
				{
					if (m.region().mouseOver()) m.region().draw(Color(Palette::Green, 128)).drawFrame(1 / 16.0, Palette::Green);
					if (m.region().leftClicked()) f->forklift.set(&m);
				}

				//Gearboxの描画
				if (ui.uiMode == UIMode::EditGearbox)
				{
					for (auto& g : m.gearboxes)
					{
						auto color = Color(Palette::Red, 128);
						RectF(0.5, 0.5).setCenter(g.centerPos()).draw(color);
						for (auto& cg : g.connectedGearbox) Line(g.centerPos(), cg->centerPos()).draw(1 / 8.0, color);
					}
					for (auto& g : m.gearboxes)
					{
						Line(g.centerPos(), g.centerPos().movedBy(0, -g.pressure / 100.0)).draw(1 / 4.0, Color(Palette::Blue));
						Line(g.centerPos(), g.centerPos().movedBy(0, +g.resistance / 100.0)).draw(1 / 4.0, Color(Palette::Orange));
					}
				}
			}
		}

		//Itemの描画
		for (auto& i : f->items)
		{
			if (i.enabled) i.texture.resized(1.0, 1.0).draw(i.pos());
		}

		//Pipeの描画
		if (ui.uiMode == UIMode::EditPipeMode)
		{
			Window::ClientRect().draw(Color(0, 128));
			for (auto& p : f->pipes)
			{
				if (p.enabled)
				{
					int value = 0;
					if (f->getTile(p.pos.movedBy(1, 0))->pipe != nullptr) value += 1;
					if (f->getTile(p.pos.movedBy(0, 1))->pipe != nullptr) value += 2;
					if (f->getTile(p.pos.movedBy(-1, 0))->pipe != nullptr) value += 4;
					if (f->getTile(p.pos.movedBy(0, -1))->pipe != nullptr) value += 8;
					texture(U"assets/pipe.png")(value * 34 + 1, 1, 32, 32).resized(1.0, 1.0).draw(p.pos, Color(Palette::White, 128));
					Rect(p.pos, Size(1, 1)).draw(Color(HSV(60 * int(p.plumber - &f->plumbers.front())), 128));
				}
			}
		}
		if (ui.uiMode == UIMode::EditMachineMode)
		{
			for (auto& p : f->pipes)
			{
				if (p.enabled)
				{
					int value = 0;
					if (f->getTile(p.pos.movedBy(1, 0))->pipe != nullptr) value += 1;
					if (f->getTile(p.pos.movedBy(0, 1))->pipe != nullptr) value += 2;
					if (f->getTile(p.pos.movedBy(-1, 0))->pipe != nullptr) value += 4;
					if (f->getTile(p.pos.movedBy(0, -1))->pipe != nullptr) value += 8;
					texture(U"assets/pipe.png")(value * 34 + 1, 1, 32, 32).resized(1.0, 1.0).draw(p.pos, Color(Palette::White, 64));
				}
			}
		}

		//フォークリフト
		f->forklift.update();

		//Wire系の描画
		if (ui.uiMode == UIMode::EditWireMode)
		{
			auto* mon = f->mouseOverNode();
			auto* sn = f->selectedNode;

			for (auto& w : f->wires) w.getBezier3().draw(1 / 8.0, w.color);

			for (auto& m : f->machines)
			{
				if (m.enabled)
				{
					for (auto& n : m.nodes)
					{
						Color color = Color(0, 0);
						Circle circle(n.pos(), 0.1250);
						switch (n.state)
						{
						case NodeState::Hi: color = Palette::Red;	break;
						case NodeState::Low: color = Palette::Blue;	break;
						case NodeState::None: color = Palette::White;	break;
						}
						circle.draw(color).drawFrame(0.0625, 0, Palette::Black);
						if (circle.mouseOver()) circle.draw(Color(0, 128));
					}
				}
			}

			if (sn != nullptr && sn != mon)
			{
				Vec2 p0 = sn->pos();
				Vec2 p1 = sn->machine->pos + sn->machine->transformInMachinePos(sn->inMachinePos + sn->deltaVector);
				if (mon != nullptr)
				{
					Vec2 p2 = mon->machine->pos + mon->machine->transformInMachinePos(mon->inMachinePos + mon->deltaVector);
					Vec2 p3 = mon->pos();
					Bezier3(p0, p1, p2, p3).draw(1 / 8.0, f->selectedWireColor);
				}
				else
				{
					Vec2 p2 = Cursor::PosF();
					Vec2 p3 = Cursor::PosF();
					Bezier3(p0, p1, p2, p3).draw(1 / 12.0, f->selectedWireColor);
				}
			}
			if (MouseL.down() && !f->ui.mouseOver) f->selectedNode = f->mouseOverNode();
			if (MouseL.up() && f->selectedNode != nullptr)
			{
				if (mon != nullptr && mon != f->selectedNode)
				{
					f->wires.emplace_back(f->selectedNode, mon, f->selectedWireColor);
				}
				f->selectedNode = nullptr;
			}

			//削除の場合
			if (MouseR.down()) f->rightClickedPos = Cursor::PosF();
			Line deleteLine(f->rightClickedPos, Cursor::PosF());
			if (MouseR.pressed())
			{
				for (auto& w : f->wires)
				{
					auto ls = w.getBezier3().getLineString();
					for (int i = 0; i < int(ls.size() - 1); i++)
					{
						if (Line(ls[i], ls[i + 1]).intersects(deleteLine))
						{
							w.getBezier3().draw(1 / 4.0, Palette::Red);
							break;
						}
					}
				}
				deleteLine.draw(1 / 16.0, Palette::Red);
			}
			if (MouseR.up())
			{
				f->wires.remove_if([&deleteLine](const Wire& w) {
					auto ls = w.getBezier3().getLineString();
					for (int i = 0; i < int(ls.size() - 1); i++)
					{
						if (Line(ls[i], ls[i + 1]).intersects(deleteLine)) return true;
					}
					return false;
				});
			}
		}
	}

	//ゴミ箱の描画
	if (f->forklift.enabled)
	{
		auto r = Forklift::trashboxRegion();
		if (r.mouseOver()) texture(U"assets/trashbox.png")(64, 0, 64, 64).resized(r.size).draw(r.pos);
		else texture(U"assets/trashbox.png")(0, 0, 64, 64).resized(r.size).draw(r.pos);
	}

	//UIの表示
	{
		bool mouseOver = false;
		{
			Rect rect(480, 48);
			if (rect.mouseOver()) mouseOver = true;
			rect.draw(Color(80)).drawFrame(4, Color(60));
			font(32)(f->name).draw(2, 2, Color(240));
		}
		{
			Rect rect1(0, 48, 240, 32);
			if (rect1.mouseOver()) mouseOver = true;
			rect1.draw(Color(80)).drawFrame(4, Color(60));
			for (int i = 0; i < 5; i++)
			{
				Rect rect2(16 + i * 48, 48, 32, 32);
				if (rect2.mouseOver()) mouseOver = true;
				if (int(ui.uiMode) - 1 == i || Rect(16 + i * 48, 48, 32, 32).mouseOver())
				{
					ui.uiTexture(32 * i, 32, 32, 32).draw(rect2.pos);

					if (rect2.leftClicked())
					{
						if (int(ui.uiMode) - 1 == i) ui.uiMode = UIMode::None;
						else ui.uiMode = UIMode(i + 1);
					}
				}
				else ui.uiTexture(32 * i, 0, 32, 32).draw(rect2.pos);
			}
		}
		switch (ui.uiMode)
		{
		case UIMode::None:
			break;
		case UIMode::EditMachineMode:

			if (ui.selectedGroup == nullptr) ui.selectedGroup = &groups.front();
			for (int i = 0; i < int(groups.size()); i++)
			{
				auto& gr = groups[i];
				Rect rect1(0, 96 + i * 32, 240, 24);
				if (rect1.mouseOver()) mouseOver = true;
				rect1.draw(rect1.mouseOver() ? Color(160) : &gr == ui.selectedGroup ? Color(120) : Color(80)).drawFrame(2, Color(60));
				font(16)(gr.name).draw(rect1.pos.movedBy(0, 0));
				if (rect1.leftClicked()) ui.selectedGroup = &gr;
				if (&gr == ui.selectedGroup)
				{
					for (int j = 0; j < int(gr.blueprints.size()); j++)
					{
						auto* b = gr.blueprints[j];
						Rect rect2(256, 96 + i * 32 + j * 32, 240, 24);
						if (rect2.mouseOver()) mouseOver = true;
						rect2.draw(rect2.mouseOver() ? Color(160) : Color(80)).drawFrame(2, Color(60));
						font(16)(b->name).draw(rect2.pos.movedBy(0, 0));
						if (rect2.leftClicked())
						{
							//登録
							f->forklift.set(b);
						}
					}
				}
			}
			break;
		case UIMode::EditPipeMode:
		{
			if (!f->ui.mouseOver)
			{
				//視点移動を適用
				auto t = f->camera.createTransformer();
				auto* tile = f->getTile(Cursor::Pos());

				if (MouseL.pressed() && tile != nullptr && tile->pipe == nullptr)
				{
					auto* pipe = f->newPipe();
					pipe->enabled = true;
					pipe->setPos(Cursor::Pos());
				}
				if (MouseR.pressed() && tile != nullptr && tile->pipe != nullptr)
				{
					auto nps = tile->pipe->nearPipes();
					tile->pipe->plumber->removePipe(tile->pipe);
					tile->pipe->enabled = false;
					tile->pipe = nullptr;

					for (auto* np : nps)
					{
						auto* plumber = f->newPlumber();
						Array<Pipe*> ps;
						plumber->addPipe(np);
						ps.emplace_back(np);
						for (int i = 0; i < int(ps.size()); i++)
						{
							auto* p = ps[i];
							for (auto* cp : p->nearPipes())
							{
								if (cp->plumber != plumber)
								{
									plumber->addPipe(cp);
									ps.emplace_back(cp);
								}
							}
						}
					}
				}
			}
		}
		break;
		case UIMode::EditWireMode:
		{
			Array<Color> colorList = {
				Palette::Red,
				Palette::Black,
				Palette::Green,
				Palette::Blue,
				Palette::White,
				Palette::Brown,
				Palette::Gray,
				Palette::Orange,
			};
			for (int i = 0; i < int(colorList.size()); i++)
			{
				Rect rect(24, 96 + i * 48, 32, 32);
				if (rect.mouseOver()) mouseOver = true;
				rect.draw(colorList[i]).drawFrame(4, 0, Color(60));
				if (f->selectedWireColor == colorList[i]) rect.drawFrame(4, 0, Palette::Yellow);
				if (rect.leftClicked()) f->selectedWireColor = colorList[i];
			}
		}
		break;

		case UIMode::Exit:
			ui.uiMode = UIMode::None;
			f->isMain = false;
			mainFactory = nullptr;
			sceneState = SceneState::MapView;
			break;
		default:
			break;
		}

		f->ui.mouseOver = mouseOver;
	}
}