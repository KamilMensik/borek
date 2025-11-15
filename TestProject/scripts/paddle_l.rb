# frozen_string_literal: true

# Player1 paddle
class PaddleL < Borek::Scriptable
  def on_update(delta)
    axis = 0
    axis += 1 if Borek::Input.key_pressed? :w
    axis -= 1 if Borek::Input.key_pressed? :s
    transform.position.y += axis * 600 * delta
  end
end
